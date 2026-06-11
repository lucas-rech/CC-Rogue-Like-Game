#include "game.h"

#include "combat.h"
#include "map.h"

#include <algorithm>
#include <cstdlib>
#include <cmath>

using namespace std;

namespace {
int distanciaManhattan(Posicao a, Posicao b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

bool posicaoOcupadaPorInimigo(const Mapa& mapa, Posicao posicao) {
    for (size_t i = 0; i < mapa.inimigos.size(); i++) {
        if (mapa.inimigos[i].vivo && mesmaPosicao(mapa.inimigos[i].posicao, posicao)) {
            return true;
        }
    }
    return false;
}
}

void inicializarJogo(Jogo& jogo) {
    inicializarJogador(jogo.jogador);
    jogo.pontuacao = {0, 0, 0, 0, 0, 0, 0, 0};
    jogo.mensagem = "Explore a masmorra. Pressione H para ajuda.";
    jogo.rodando = true;
    jogo.venceu = false;
    jogo.perdeu = false;
    carregarNivel(jogo, 1);
}

void processarEntrada(Jogo& jogo, char tecla) {
    if (jogo.jogador.pontosDisponiveis > 0 && tecla >= '1' && tecla <= '4') {
        distribuirPontoAtributo(jogo.jogador, tecla);
        jogo.mensagem = "Atributo aumentado.";
        return;
    }

    switch (tecla) {
    case 'p':
    case 'P':
        usarPocao(jogo);
        break;
    case 'f':
    case 'F':
        atacarInimigoAdjacente(jogo);
        break;
    case 'h':
    case 'H':
        jogo.mensagem = "WASD move, F ataca, P usa pocao, 1-4 atributos, Q sai.";
        break;
    case 'q':
    case 'Q':
        jogo.rodando = false;
        jogo.perdeu = true;
        jogo.mensagem = "Voce abandonou a masmorra.";
        break;
    default:
        // Outras teclas ignoradas no modo em tempo real
        break;
    }
}

bool atacarInimigoAdjacente(Jogo& jogo) {
    if (jogo.jogador.tempoAtaque > 0.0f) {
        return false; // Em cooldown
    }

    // Procura o inimigo vivo mais proximo dentro do raio de ataque
    Inimigo* alvo = nullptr;
    float menorDist = 120.0f; // Alcance do ataque (pixel)

    for (size_t i = 0; i < jogo.mapa.inimigos.size(); i++) {
        Inimigo& inimigo = jogo.mapa.inimigos[i];
        if (inimigo.vivo) {
            float dx = inimigo.x - jogo.jogador.x;
            float dy = inimigo.y - jogo.jogador.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < menorDist) {
                menorDist = dist;
                alvo = &inimigo;
            }
        }
    }

    if (alvo != nullptr) {
        atacarInimigo(jogo, *alvo);
        verificarCondicoesFim(jogo);
        return true;
    }

    jogo.mensagem = "Nao ha inimigos proximos para atacar.";
    return false;
}

bool tentarMoverJogador(Jogo& jogo, int deslocamentoX, int deslocamentoY) {
    // Stub para compatibilidade, o movimento principal ocorre no loop real-time
    return false;
}

void atualizarInimigos(Jogo& jogo, float dt) {
    float radius = 24.0f;
    for (size_t i = 0; i < jogo.mapa.inimigos.size(); i++) {
        Inimigo& inimigo = jogo.mapa.inimigos[i];
        if (!inimigo.vivo) {
            continue;
        }

        // Vetor de direcao para o jogador
        float dx = jogo.jogador.x - inimigo.x;
        float dy = jogo.jogador.y - inimigo.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < 60.0f) {
            // Muito proximo: ataca o jogador
            if (inimigo.tempoAtaque <= 0.0f) {
                inimigoAtacaJogador(jogo, inimigo);
                inimigo.tempoAtaque = 1.2f; // Cooldown de ataque de 1.2s
            }
        } else if (dist < 600.0f) {
            // Dentro do raio de deteccao: persegue o jogador
            float speed = 100.0f;
            if (inimigo.tipo == INIMIGO_PERSEGUIDOR) {
                speed = 140.0f;
            } else if (inimigo.tipo == INIMIGO_BOSS) {
                speed = 130.0f;
            } else if (inimigo.tipo == INIMIGO_TANQUE) {
                speed = 65.0f;
            }

            inimigo.x += (dx / dist) * speed * dt;
            inimigo.y += (dy / dist) * speed * dt;

            // Resolucao de colisoes com a parede para o inimigo
            int exGrid = static_cast<int>(inimigo.x / 80.0f);
            int eyGrid = static_cast<int>(inimigo.y / 80.0f);

            for (int y = eyGrid - 2; y <= eyGrid + 2; ++y) {
                for (int x = exGrid - 2; x <= exGrid + 2; ++x) {
                    if (x >= 0 && x < jogo.mapa.largura && y >= 0 && y < jogo.mapa.altura) {
                        if (jogo.mapa.tiles[y][x].bloqueiaMovimento) {
                            float tileLeft = x * 80.0f;
                            float tileRight = tileLeft + 80.0f;
                            float tileTop = y * 80.0f;
                            float tileBottom = tileTop + 80.0f;

                            float closestX = std::max(tileLeft, std::min(inimigo.x, tileRight));
                            float closestY = std::max(tileTop, std::min(inimigo.y, tileBottom));

                            float diffX = inimigo.x - closestX;
                            float diffY = inimigo.y - closestY;
                            float distSq = diffX * diffX + diffY * diffY;

                            if (distSq < radius * radius && distSq > 0.0001f) {
                                float d = std::sqrt(distSq);
                                float overlap = radius - d;
                                inimigo.x += (diffX / d) * overlap;
                                inimigo.y += (diffY / d) * overlap;
                            }
                        }
                    }
                }
            }

            // Manter inimigo nos limites validos da masmorra
            float minX = 80.0f + radius;
            float maxX = (jogo.mapa.largura - 1) * 80.0f - radius;
            float minY = 80.0f + radius;
            float maxY = (jogo.mapa.altura - 1) * 80.0f - radius;
            if (inimigo.x < minX) inimigo.x = minX;
            if (inimigo.x > maxX) inimigo.x = maxX;
            if (inimigo.y < minY) inimigo.y = minY;
            if (inimigo.y > maxY) inimigo.y = maxY;

            // Atualiza posicao no grid
            inimigo.posicao.x = static_cast<int>(inimigo.x / 80.0f);
            inimigo.posicao.y = static_cast<int>(inimigo.y / 80.0f);
        }
    }

    // Resolucao de colisao entre inimigos (evita sobreposicao)
    for (size_t i = 0; i < jogo.mapa.inimigos.size(); ++i) {
        if (!jogo.mapa.inimigos[i].vivo) continue;
        for (size_t j = i + 1; j < jogo.mapa.inimigos.size(); ++j) {
            if (!jogo.mapa.inimigos[j].vivo) continue;
            float ex = jogo.mapa.inimigos[i].x - jogo.mapa.inimigos[j].x;
            float ey = jogo.mapa.inimigos[i].y - jogo.mapa.inimigos[j].y;
            float distSq = ex * ex + ey * ey;
            float minDist = 48.0f; // 2 * raio de colisao
            if (distSq < minDist * minDist && distSq > 0.0001f) {
                float d = std::sqrt(distSq);
                float overlap = minDist - d;
                // Empurra ambos na direcao oposta
                jogo.mapa.inimigos[i].x += (ex / d) * overlap * 0.5f;
                jogo.mapa.inimigos[i].y += (ey / d) * overlap * 0.5f;
                jogo.mapa.inimigos[j].x -= (ex / d) * overlap * 0.5f;
                jogo.mapa.inimigos[j].y -= (ey / d) * overlap * 0.5f;
            }
        }
    }
}

void atualizarJogoRealTime(Jogo& jogo, float dt, float dx, float dy) {
    if (!jogo.rodando) return;

    // 1. Mover jogador
    float speed = 250.0f; // Pixels por segundo
    jogo.jogador.x += dx * speed * dt;
    jogo.jogador.y += dy * speed * dt;

    // 2. Resolver colisoes deslizantes contra as paredes
    float radius = 24.0f;
    int gridX = static_cast<int>(jogo.jogador.x / 80.0f);
    int gridY = static_cast<int>(jogo.jogador.y / 80.0f);

    for (int y = gridY - 2; y <= gridY + 2; ++y) {
        for (int x = gridX - 2; x <= gridX + 2; ++x) {
            if (x >= 0 && x < jogo.mapa.largura && y >= 0 && y < jogo.mapa.altura) {
                if (jogo.mapa.tiles[y][x].bloqueiaMovimento) {
                    float tileLeft = x * 80.0f;
                    float tileRight = tileLeft + 80.0f;
                    float tileTop = y * 80.0f;
                    float tileBottom = tileTop + 80.0f;

                    float closestX = std::max(tileLeft, std::min(jogo.jogador.x, tileRight));
                    float closestY = std::max(tileTop, std::min(jogo.jogador.y, tileBottom));

                    float diffX = jogo.jogador.x - closestX;
                    float diffY = jogo.jogador.y - closestY;
                    float distSq = diffX * diffX + diffY * diffY;

                    if (distSq < radius * radius) {
                        // Verifica se e porta e o jogador pode destranca-la
                        if (jogo.mapa.tiles[y][x].tipo == PORTA) {
                            if (jogo.jogador.chaves > 0) {
                                jogo.jogador.chaves--;
                                jogo.mapa.tiles[y][x].tipo = CHAO;
                                jogo.mapa.tiles[y][x].bloqueiaMovimento = false;
                                jogo.mapa.matriz[y][x] = CODIGO_CHAO;
                                jogo.mensagem = "Voce abriu a porta com uma chave.";
                                continue;
                            } else {
                                jogo.mensagem = "A porta esta trancada. Procure uma chave.";
                            }
                        }

                        // Empurra o jogador para fora da parede
                        if (distSq > 0.0001f) {
                            float dist = std::sqrt(distSq);
                            float overlap = radius - dist;
                            jogo.jogador.x += (diffX / dist) * overlap;
                            jogo.jogador.y += (diffY / dist) * overlap;
                        } else {
                            jogo.jogador.x += radius;
                        }
                    }
                }
            }
        }
    }

    // Limites absolutos do mapa
    float minX = 80.0f + radius;
    float maxX = (jogo.mapa.largura - 1) * 80.0f - radius;
    float minY = 80.0f + radius;
    float maxY = (jogo.mapa.altura - 1) * 80.0f - radius;
    if (jogo.jogador.x < minX) jogo.jogador.x = minX;
    if (jogo.jogador.x > maxX) jogo.jogador.x = maxX;
    if (jogo.jogador.y < minY) jogo.jogador.y = minY;
    if (jogo.jogador.y > maxY) jogo.jogador.y = maxY;

    // 3. Atualizar a posicao inteira no grid e triggers
    int newGridX = static_cast<int>(jogo.jogador.x / 80.0f);
    int newGridY = static_cast<int>(jogo.jogador.y / 80.0f);

    if (newGridX != jogo.jogador.posicao.x || newGridY != jogo.jogador.posicao.y) {
        jogo.jogador.posicao = {newGridX, newGridY};

        // Conta movimentos proporcionalmente
        jogo.jogador.movimentos++;
        jogo.pontuacao.movimentos++;

        // Coleta itens na celula
        Item* item = buscarItemNaPosicao(jogo.mapa, jogo.jogador.posicao);
        if (item != nullptr) {
            coletarItem(jogo, *item);
        }

        // Ativa armadilhas
        if (jogo.mapa.tiles[jogo.jogador.posicao.y][jogo.jogador.posicao.x].tipo == ARMADILHA) {
            ativarArmadilha(jogo, jogo.jogador.posicao);
        }

        // Avanco de nivel
        if (jogo.mapa.tiles[jogo.jogador.posicao.y][jogo.jogador.posicao.x].tipo == SAIDA) {
            if (jogo.mapa.nivelAtual < TOTAL_NIVEIS) {
                jogo.pontuacao.niveisConcluidos++;
                carregarNivel(jogo, jogo.mapa.nivelAtual + 1);
                jogo.mensagem = "Voce desceu para o proximo nivel.";
            } else {
                jogo.mensagem = "Derrote o chefe final antes de sair.";
            }
        }

        atualizarCampoDeVisao(jogo);
        verificarCondicoesFim(jogo);
    }

    // 4. Trigger de conversa automatica com NPCs por proximidade
    for (size_t i = 0; i < jogo.mapa.npcs.size(); i++) {
        NPC& npc = jogo.mapa.npcs[i];
        if (!npc.interagiu) {
            float dxNPC = jogo.jogador.x - (npc.posicao.x * 80.0f + 40.0f);
            float dyNPC = jogo.jogador.y - (npc.posicao.y * 80.0f + 40.0f);
            float distNPC = std::sqrt(dxNPC * dxNPC + dyNPC * dyNPC);
            if (distNPC < 96.0f) {
                interagirComNPC(jogo, npc);
            }
        }
    }

    // 5. Atualizar Inimigos em tempo real
    atualizarInimigos(jogo, dt);
}

void coletarItem(Jogo& jogo, Item& item) {
    item.coletado = true;
    jogo.jogador.itensColetados++;
    jogo.pontuacao.itensColetados++;

    if (item.tipo == ITEM_POCAO) {
        jogo.jogador.pocoes++;
        jogo.mensagem = "Voce coletou uma pocao.";
    } else if (item.tipo == ITEM_CHAVE) {
        jogo.jogador.chaves += item.valor;
        jogo.mensagem = "Voce coletou uma chave.";
    } else {
        aplicarEfeitoDoItem(jogo.jogador, item.tipo, item.valor);
        jogo.mensagem = "Voce coletou um equipamento.";
    }
}

void usarPocao(Jogo& jogo) {
    if (jogo.jogador.pocoes <= 0) {
        jogo.mensagem = "Voce nao tem pocoes.";
        return;
    }

    jogo.jogador.pocoes--;
    jogo.pontuacao.pocoesUsadas++;
    jogo.jogador.status.vida = min(jogo.jogador.status.vidaMaxima, jogo.jogador.status.vida + 22);
    jogo.mensagem = "Voce usou uma pocao.";
}

bool tentarAbrirPorta(Jogo& jogo, Posicao posicao) {
    if (jogo.jogador.chaves <= 0) {
        jogo.mensagem = "A porta esta trancada. Procure uma chave.";
        return false;
    }

    jogo.jogador.chaves--;
    jogo.mapa.tiles[posicao.y][posicao.x].tipo = CHAO;
    jogo.mapa.tiles[posicao.y][posicao.x].bloqueiaMovimento = false;
    jogo.mapa.matriz[posicao.y][posicao.x] = CODIGO_CHAO;
    jogo.mensagem = "Voce abriu a porta com uma chave.";
    return true;
}

void ativarArmadilha(Jogo& jogo, Posicao posicao) {
    int dano = 8 + jogo.mapa.nivelAtual * 2;
    jogo.jogador.status.vida -= dano;
    jogo.pontuacao.danoRecebido += dano;
    jogo.mensagem = "Armadilha! Voce sofreu " + to_string(dano) + " de dano.";
    alterarMapaPorEvento(jogo, posicao);
}

void aplicarEfeitoDoItem(Jogador& jogador, TipoItem tipo, int valor) {
    if (tipo == ITEM_ARMA) {
        jogador.atributos.forca += valor;
    } else if (tipo == ITEM_ESCUDO) {
        jogador.atributos.defesa += valor;
    } else if (tipo == ITEM_POWERUP) {
        jogador.atributos.agilidade += valor;
        jogador.atributos.vitalidade += valor;
    }
    recalcularStatus(jogador);
}

void interagirComNPC(Jogo& jogo, NPC& NPC) {
    alterarMapaPorNPC(jogo, NPC);
}

void atualizarPontuacao(Jogo& jogo) {
    jogo.pontuacao.pontuacaoFinal = calcularPontuacaoFinal(jogo);
}

int calcularPontuacaoFinal(const Jogo& jogo) {
    int pontuacaoCalculada = 0;
    pontuacaoCalculada += jogo.pontuacao.inimigosDerrotados * 100;
    pontuacaoCalculada += jogo.pontuacao.bossDerrotado * 500;
    pontuacaoCalculada += jogo.pontuacao.itensColetados * 50;
    pontuacaoCalculada += jogo.pontuacao.niveisConcluidos * 200;
    pontuacaoCalculada -= jogo.pontuacao.movimentos;
    pontuacaoCalculada -= jogo.pontuacao.pocoesUsadas * 20;
    pontuacaoCalculada -= jogo.pontuacao.danoRecebido * 5;
    pontuacaoCalculada += max(0, jogo.jogador.status.vida) * 10;
    return max(0, pontuacaoCalculada);
}

void verificarCondicoesFim(Jogo& jogo) {
    if (jogo.jogador.status.vida <= 0) {
        jogo.jogador.vivo = false;
        jogo.perdeu = true;
        jogo.rodando = false;
        jogo.mensagem = "Sua vida chegou a zero.";
    }
    atualizarPontuacao(jogo);
}
