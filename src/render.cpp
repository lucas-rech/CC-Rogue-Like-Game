#include "render.h"
#include "game.h"
#include "map.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <string>

namespace {
// Utilitario de texto
void desenharTexto(sf::RenderWindow& window, const sf::Font& font, const std::string& str, float x, float y, unsigned int size, sf::Color cor, bool bold = false) {
    sf::Text text;
    text.setFont(font);
    text.setString(sf::String::fromUtf8(str.begin(), str.end()));
    text.setCharacterSize(size);
    text.setFillColor(cor);
    text.setPosition(x, y);
    if (bold) {
        text.setStyle(sf::Text::Bold);
    }
    window.draw(text);
}

// Utilitario de barra de status
void desenharBarra(sf::RenderWindow& window, float x, float y, float largura, float altura, float valorAtual, float valorMaximo, sf::Color corPreenchimento, sf::Color corFundo) {
    // Fundo da barra
    sf::RectangleShape fundo(sf::Vector2f(largura, altura));
    fundo.setFillColor(corFundo);
    fundo.setPosition(x, y);
    window.draw(fundo);

    // Preenchimento proporcional
    if (valorMaximo > 0 && valorAtual > 0) {
        float proporcao = valorAtual / valorMaximo;
        if (proporcao > 1.0f) proporcao = 1.0f;
        sf::RectangleShape preenchimento(sf::Vector2f(largura * proporcao, altura));
        preenchimento.setFillColor(corPreenchimento);
        preenchimento.setPosition(x, y);
        window.draw(preenchimento);
    }

    // Borda
    sf::RectangleShape borda(sf::Vector2f(largura, altura));
    borda.setFillColor(sf::Color::Transparent);
    borda.setOutlineThickness(1.0f);
    borda.setOutlineColor(sf::Color(100, 116, 139));
    borda.setPosition(x, y);
    window.draw(borda);
}

int codigoPorInimigo(TipoInimigo tipo) {
    if (tipo == INIMIGO_ALEATORIO) return CODIGO_INIMIGO_COMUM;
    if (tipo == INIMIGO_PERSEGUIDOR) return CODIGO_INIMIGO_PERSEGUIDOR;
    if (tipo == INIMIGO_TANQUE) return CODIGO_INIMIGO_TANQUE;
    return CODIGO_BOSS;
}

int codigoPorItem(TipoItem tipo) {
    if (tipo == ITEM_POCAO) return CODIGO_ITEM_POCAO;
    if (tipo == ITEM_CHAVE) return CODIGO_ITEM_CHAVE;
    if (tipo == ITEM_ARMA) return CODIGO_ITEM_ARMA;
    if (tipo == ITEM_ESCUDO) return CODIGO_ITEM_ESCUDO;
    return CODIGO_ITEM_POWERUP;
}
}

void configurarConsole() {
    // Dummy para compatibilidade
}

void renderizarJogoSFML(sf::RenderWindow& window, const Jogo& jogo, const sf::Font& font, const TexturasJogo& texturas) {
    // Relogio para animar os frames dos orcs em tempo real
    static sf::Clock animClock;
    int frameIdx = (animClock.getElapsedTime().asMilliseconds() / 150) % 4; // 4 colunas no idle

    // Fundo da janela inteira (Deep blue-black)
    sf::RectangleShape bg(sf::Vector2f(1920.f, 1080.f));
    bg.setFillColor(sf::Color(11, 15, 25));
    window.draw(bg);

    // --- CONFIGURAÇÃO DA CÂMERA DO JOGO (VIEWPORT) ---
    float viewW = 1660.f;
    float viewH = 850.f; // 900 - 50 (Header space)

    // Clampar a câmera para não mostrar o vazio fora dos limites do mapa
    float minCamX = viewW / 2.f;
    float maxCamX = jogo.mapa.largura * 80.f - viewW / 2.f;
    float minCamY = viewH / 2.f;
    float maxCamY = jogo.mapa.altura * 80.f - viewH / 2.f;

    // Se o mapa for menor que a câmera, centraliza no meio do mapa
    float camX = jogo.jogador.x;
    float camY = jogo.jogador.y;
    
    if (jogo.mapa.largura * 80.f < viewW) {
        camX = (jogo.mapa.largura * 80.f) / 2.f;
    } else {
        if (camX < minCamX) camX = minCamX;
        if (camX > maxCamX) camX = maxCamX;
    }

    if (jogo.mapa.altura * 80.f < viewH) {
        camY = (jogo.mapa.altura * 80.f) / 2.f;
    } else {
        if (camY < minCamY) camY = minCamY;
        if (camY > maxCamY) camY = maxCamY;
    }

    sf::View gameView(sf::Vector2f(camX, camY), sf::Vector2f(viewW, viewH));
    gameView.setViewport(sf::FloatRect(0.f, 50.f / 1080.f, viewW / 1920.f, viewH / 1080.f));
    window.setView(gameView);

    // --- CULLING E RENDERIZAÇÃO DOS TILES DO MAPA ---
    float viewLeft = camX - viewW / 2.f;
    float viewRight = camX + viewW / 2.f;
    float viewTop = camY - viewH / 2.f;
    float viewBottom = camY + viewH / 2.f;

    int startX = std::max(0, static_cast<int>(viewLeft / 80.f));
    int endX = std::min(jogo.mapa.largura - 1, static_cast<int>(viewRight / 80.f));
    int startY = std::max(0, static_cast<int>(viewTop / 80.f));
    int endY = std::min(jogo.mapa.altura - 1, static_cast<int>(viewBottom / 80.f));

    const float tileSize = 80.0f;

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            Tile tile = jogo.mapa.tiles[y][x];
            float px = x * tileSize;
            float py = y * tileSize;

            if (!tile.explorado) {
                sf::RectangleShape ret(sf::Vector2f(tileSize, tileSize));
                ret.setFillColor(sf::Color::Black);
                ret.setPosition(px, py);
                window.draw(ret);
                continue;
            }

            sf::Color tileColor;
            sf::Color outlineColor = sf::Color::Transparent;

            if (tile.tipo == PAREDE) {
                tileColor = tile.visivel ? sf::Color(71, 85, 105) : sf::Color(30, 41, 59); // Slate-600 vs Slate-800
                outlineColor = tile.visivel ? sf::Color(100, 116, 139) : sf::Color(15, 23, 42);
            } else if (tile.tipo == PORTA) {
                tileColor = tile.visivel ? sf::Color(180, 83, 9) : sf::Color(69, 26, 3); // Brown-700 vs Brown-900
            } else if (tile.tipo == ARMADILHA) {
                tileColor = tile.visivel ? sf::Color(220, 38, 38) : sf::Color(15, 23, 42);
            } else if (tile.tipo == SAIDA) {
                tileColor = tile.visivel ? sf::Color(124, 58, 237) : sf::Color(59, 7, 100); // Purple
            } else { // CHAO
                tileColor = tile.visivel ? sf::Color(30, 41, 59) : sf::Color(15, 23, 42);
            }

            sf::RectangleShape ret(sf::Vector2f(tileSize, tileSize));
            ret.setFillColor(tileColor);
            if (outlineColor != sf::Color::Transparent) {
                ret.setOutlineThickness(1.0f);
                ret.setOutlineColor(outlineColor);
            }
            ret.setPosition(px, py);
            window.draw(ret);

            if (tile.visivel) {
                if (tile.tipo == ARMADILHA) {
                    desenharTexto(window, font, "X", px + 30.f, py + 20.f, 32, sf::Color::White, true);
                }
                if (tile.tipo == PORTA) {
                    sf::CircleShape knob(8.0f);
                    knob.setFillColor(sf::Color::Yellow);
                    knob.setPosition(px + tileSize/2.f - 4.0f, py + tileSize/2.f - 4.0f);
                    window.draw(knob);
                }
            }
        }
    }

    // --- RENDERIZAÇÃO DE ITENS E NPCs ---
    // Itens
    for (size_t i = 0; i < jogo.mapa.itens.size(); i++) {
        const Item& item = jogo.mapa.itens[i];
        if (!item.coletado && jogo.mapa.tiles[item.posicao.y][item.posicao.x].visivel) {
            float px = item.posicao.x * tileSize + 40.f;
            float py = item.posicao.y * tileSize + 40.f;

            sf::CircleShape itemShape(20.0f);
            itemShape.setOrigin(20.f, 20.f);
            sf::Color itemColor;

            if (item.tipo == ITEM_POCAO) itemColor = sf::Color(34, 197, 94);
            else if (item.tipo == ITEM_CHAVE) itemColor = sf::Color(234, 179, 8);
            else if (item.tipo == ITEM_ARMA) itemColor = sf::Color(239, 68, 68);
            else if (item.tipo == ITEM_ESCUDO) itemColor = sf::Color(59, 130, 246);
            else itemColor = sf::Color(168, 85, 247);

            itemShape.setFillColor(itemColor);
            itemShape.setPosition(px, py);
            window.draw(itemShape);
        }
    }

    // NPCs
    for (size_t i = 0; i < jogo.mapa.npcs.size(); i++) {
        const NPC& npc = jogo.mapa.npcs[i];
        if (jogo.mapa.tiles[npc.posicao.y][npc.posicao.x].visivel) {
            float px = npc.posicao.x * tileSize + 40.f;
            float py = npc.posicao.y * tileSize + 40.f;

            sf::CircleShape npcShape(30.0f);
            npcShape.setOrigin(30.f, 30.f);
            npcShape.setFillColor(sf::Color(6, 182, 212));
            npcShape.setOutlineThickness(2.0f);
            npcShape.setOutlineColor(sf::Color(8, 145, 178));
            npcShape.setPosition(px, py);
            window.draw(npcShape);
        }
    }

    // --- RENDERIZAÇÃO DOS INIMIGOS ---
    for (size_t i = 0; i < jogo.mapa.inimigos.size(); i++) {
        const Inimigo& inimigo = jogo.mapa.inimigos[i];
        if (inimigo.vivo && jogo.mapa.tiles[inimigo.posicao.y][inimigo.posicao.x].visivel) {
            bool carregado = false;
            bool carregadoAtaque = false;
            const sf::Texture* tex = nullptr;
            const sf::Texture* texAtaque = nullptr;
            float scaleMult = 1.35f;

            if (inimigo.tipo == INIMIGO_ALEATORIO) {
                carregado = texturas.comumCarregado;
                tex = &texturas.inimigoComum;
                carregadoAtaque = texturas.comumAtaqueCarregado;
                texAtaque = &texturas.comumAtaque;
            } else if (inimigo.tipo == INIMIGO_PERSEGUIDOR) {
                carregado = texturas.perseguidorCarregado;
                tex = &texturas.inimigoPerseguidor;
                carregadoAtaque = texturas.perseguidorAtaqueCarregado;
                texAtaque = &texturas.perseguidorAtaque;
            } else if (inimigo.tipo == INIMIGO_TANQUE) {
                carregado = texturas.tanqueCarregado;
                tex = &texturas.inimigoTanque;
                carregadoAtaque = texturas.tanqueAtaqueCarregado;
                texAtaque = &texturas.tanqueAtaque;
            } else { // BOSS
                carregado = texturas.bossCarregado;
                tex = &texturas.boss;
                carregadoAtaque = texturas.bossAtaqueCarregado;
                texAtaque = &texturas.bossAtaque;
                scaleMult = 1.8f;
            }

            float finalScale = (tileSize / 64.f) * scaleMult;

            if (inimigo.tempoAtaque > 0.0f) {
                if (carregadoAtaque && texAtaque != nullptr) {
                    sf::Sprite sprite(*texAtaque);
                    int frameAttack = static_cast<int>(((0.4f - inimigo.tempoAtaque) / 0.4f) * 8.0f);
                    if (frameAttack > 7) frameAttack = 7;
                    if (frameAttack < 0) frameAttack = 0;
                    sprite.setTextureRect(sf::IntRect(frameAttack * 64, 0, 64, 64));
                    sprite.setScale(finalScale, finalScale);
                    sprite.setOrigin(32.f, 32.f);
                    sprite.setPosition(inimigo.x, inimigo.y - 12.f);
                    window.draw(sprite);
                } else {
                    sf::CircleShape enemyShape(inimigo.tipo == INIMIGO_BOSS ? 40.0f : 30.0f);
                    enemyShape.setOrigin(enemyShape.getRadius(), enemyShape.getRadius());
                    enemyShape.setFillColor(sf::Color::White); // Flash de ataque
                    enemyShape.setOutlineThickness(3.0f);
                    enemyShape.setOutlineColor(sf::Color::Red);
                    enemyShape.setPosition(inimigo.x, inimigo.y);
                    window.draw(enemyShape);
                }
            } else {
                if (carregado && tex != nullptr) {
                    sf::Sprite sprite(*tex);
                    sprite.setTextureRect(sf::IntRect(frameIdx * 64, 0, 64, 64));
                    sprite.setScale(finalScale, finalScale);
                    sprite.setOrigin(32.f, 32.f);
                    sprite.setPosition(inimigo.x, inimigo.y - 12.f);
                    window.draw(sprite);
                } else {
                    sf::CircleShape enemyShape(inimigo.tipo == INIMIGO_BOSS ? 40.0f : 30.0f);
                    enemyShape.setOrigin(enemyShape.getRadius(), enemyShape.getRadius());
                    sf::Color fillCol;
                    sf::Color outCol;

                    if (inimigo.tipo == INIMIGO_ALEATORIO) {
                        fillCol = sf::Color(244, 63, 94);
                        outCol = sf::Color(190, 18, 60);
                    } else if (inimigo.tipo == INIMIGO_PERSEGUIDOR) {
                        fillCol = sf::Color(132, 204, 22);
                        outCol = sf::Color(79, 128, 8);
                    } else if (inimigo.tipo == INIMIGO_TANQUE) {
                        fillCol = sf::Color(99, 102, 241);
                        outCol = sf::Color(67, 56, 202);
                    } else { // BOSS
                        fillCol = sf::Color(220, 38, 38);
                        outCol = sf::Color(127, 29, 29);
                    }

                    enemyShape.setFillColor(fillCol);
                    enemyShape.setOutlineThickness(2.0f);
                    enemyShape.setOutlineColor(outCol);
                    enemyShape.setPosition(inimigo.x, inimigo.y);
                    window.draw(enemyShape);
                }
            }
        }
    }

    // --- RENDERIZAÇÃO DO JOGADOR ---
    float playerScale = (tileSize / 64.f) * 1.35f;
    if (jogo.jogador.tempoAtaque > 0.0f) {
        if (texturas.jogadorAtaqueCarregado) {
            sf::Sprite sprite(texturas.jogadorAtaque);
            int frameAttack = static_cast<int>(((0.4f - jogo.jogador.tempoAtaque) / 0.4f) * 8.0f);
            if (frameAttack > 7) frameAttack = 7;
            if (frameAttack < 0) frameAttack = 0;
            sprite.setTextureRect(sf::IntRect(frameAttack * 64, 0, 64, 64));
            sprite.setScale(playerScale, playerScale);
            sprite.setOrigin(32.f, 32.f);
            sprite.setPosition(jogo.jogador.x, jogo.jogador.y - 12.f);
            window.draw(sprite);
        } else {
            sf::CircleShape playerShape(30.0f);
            playerShape.setOrigin(30.f, 30.f);
            playerShape.setFillColor(sf::Color::Red);
            playerShape.setOutlineThickness(3.0f);
            playerShape.setOutlineColor(sf::Color(250, 204, 21));
            playerShape.setPosition(jogo.jogador.x, jogo.jogador.y);
            window.draw(playerShape);
        }
    } else {
        if (texturas.jogadorCarregado) {
            sf::Sprite sprite(texturas.jogador);
            sprite.setTextureRect(sf::IntRect(frameIdx * 64, 0, 64, 64));
            sprite.setScale(playerScale, playerScale);
            sprite.setOrigin(32.f, 32.f);
            sprite.setPosition(jogo.jogador.x, jogo.jogador.y - 12.f);
            window.draw(sprite);
        } else {
            sf::CircleShape playerShape(30.0f);
            playerShape.setOrigin(30.f, 30.f);
            playerShape.setFillColor(sf::Color(250, 204, 21)); // Gold
            playerShape.setOutlineThickness(2.0f);
            playerShape.setOutlineColor(sf::Color(217, 119, 6));
            playerShape.setPosition(jogo.jogador.x, jogo.jogador.y);
            window.draw(playerShape);
        }
    }

    // --- RESTAURAR A VISUALIZAÇÃO PADRÃO PARA A UI ---
    window.setView(window.getDefaultView());

    // Desenhar a moldura ao redor da área de jogo em coordenadas da tela
    sf::RectangleShape molduraJogo(sf::Vector2f(viewW, viewH));
    molduraJogo.setFillColor(sf::Color::Transparent);
    molduraJogo.setOutlineThickness(2.0f);
    molduraJogo.setOutlineColor(sf::Color(51, 65, 85));
    molduraJogo.setPosition(0.f, 50.f);
    window.draw(molduraJogo);

    // Desenhar a barra superior do cabeçalho (Header)
    sf::RectangleShape headerBg(sf::Vector2f(1660.f, 50.f));
    headerBg.setFillColor(sf::Color(15, 23, 42, 220)); // Semi-transparente
    headerBg.setOutlineThickness(1.0f);
    headerBg.setOutlineColor(sf::Color(51, 65, 85));
    headerBg.setPosition(0.f, 0.f);
    window.draw(headerBg);

    desenharTexto(window, font, "CC ROGUELIKE MASMORRA", 30.f, 10.f, 24, sf::Color(251, 191, 36), true);
    desenharTexto(window, font, "Nivel " + std::to_string(jogo.mapa.nivelAtual) + " / " + std::to_string(TOTAL_NIVEIS), 1450.f, 15.f, 18, sf::Color(148, 163, 184));

    // --- PAINEL SIDEBAR HUD (Direita ajustada para 1920x1080) ---
    float hudX = 1690.0f;
    float hudY = 60.0f;

    // Titulo Status
    desenharTexto(window, font, "STATUS DO JOGADOR", hudX, hudY, 20, sf::Color(251, 191, 36), true);

    // Vida Maxima / Atual
    std::string hpStr = "Vida: " + std::to_string(jogo.jogador.status.vida) + " / " + std::to_string(jogo.jogador.status.vidaMaxima);
    desenharTexto(window, font, hpStr, hudX, hudY + 40.0f, 16, sf::Color::White);
    desenharBarra(window, hudX, hudY + 65.0f, 200.0f, 14.0f, static_cast<float>(jogo.jogador.status.vida), static_cast<float>(jogo.jogador.status.vidaMaxima), sf::Color(239, 68, 68), sf::Color(69, 10, 10));

    // Nivel e XP
    std::string levelStr = "Nivel: " + std::to_string(jogo.jogador.nivel);
    desenharTexto(window, font, levelStr, hudX, hudY + 95.0f, 16, sf::Color::White);
    std::string xpStr = "XP: " + std::to_string(jogo.jogador.xp) + " / " + std::to_string(jogo.jogador.xpProximoNivel);
    desenharTexto(window, font, xpStr, hudX + 90.0f, hudY + 95.0f, 16, sf::Color(147, 197, 253));
    desenharBarra(window, hudX, hudY + 120.0f, 200.0f, 10.0f, static_cast<float>(jogo.jogador.xp), static_cast<float>(jogo.jogador.xpProximoNivel), sf::Color(59, 130, 246), sf::Color(30, 41, 59));

    // Stats de Combate
    desenharTexto(window, font, "Ataque:   " + std::to_string(jogo.jogador.status.ataque), hudX, hudY + 145.0f, 15, sf::Color(244, 63, 94));
    desenharTexto(window, font, "Armadura: " + std::to_string(jogo.jogador.status.armadura), hudX, hudY + 165.0f, 15, sf::Color(59, 130, 246));
    desenharTexto(window, font, "Acerto:   " + std::to_string(jogo.jogador.status.acerto) + "%", hudX, hudY + 185.0f, 15, sf::Color(168, 85, 247));
    desenharTexto(window, font, "Esquiva:  " + std::to_string(jogo.jogador.status.esquiva) + "%", hudX, hudY + 205.0f, 15, sf::Color(34, 197, 94));

    // Divisor
    sf::RectangleShape line1(sf::Vector2f(200.0f, 1.0f));
    line1.setFillColor(sf::Color(51, 65, 85));
    line1.setPosition(hudX, hudY + 235.0f);
    window.draw(line1);

    // Inventario
    desenharTexto(window, font, "INVENTARIO", hudX, hudY + 250.0f, 18, sf::Color(251, 191, 36), true);
    desenharTexto(window, font, "🧪 Pocoes: " + std::to_string(jogo.jogador.pocoes), hudX, hudY + 280.0f, 16, sf::Color::White);
    desenharTexto(window, font, "🔑 Chaves:  " + std::to_string(jogo.jogador.chaves), hudX, hudY + 305.0f, 16, sf::Color::White);

    // Divisor
    sf::RectangleShape line2(sf::Vector2f(200.0f, 1.0f));
    line2.setFillColor(sf::Color(51, 65, 85));
    line2.setPosition(hudX, hudY + 335.0f);
    window.draw(line2);

    // Atributos base
    desenharTexto(window, font, "ATRIBUTOS BASE", hudX, hudY + 350.0f, 18, sf::Color(251, 191, 36), true);
    desenharTexto(window, font, "1. Forca:      " + std::to_string(jogo.jogador.atributos.forca), hudX, hudY + 380.0f, 15, sf::Color::White);
    desenharTexto(window, font, "2. Defesa:     " + std::to_string(jogo.jogador.atributos.defesa), hudX, hudY + 400.0f, 15, sf::Color::White);
    desenharTexto(window, font, "3. Agilidade:  " + std::to_string(jogo.jogador.atributos.agilidade), hudX, hudY + 420.0f, 15, sf::Color::White);
    desenharTexto(window, font, "4. Vitalidade: " + std::to_string(jogo.jogador.atributos.vitalidade), hudX, hudY + 440.0f, 15, sf::Color::White);

    // Alerta de Pontos Disponiveis
    if (jogo.jogador.pontosDisponiveis > 0) {
        sf::RectangleShape alertBg(sf::Vector2f(200.f, 35.f));
        alertBg.setFillColor(sf::Color(234, 88, 12, 100)); // Orange alpha
        alertBg.setOutlineThickness(1.0f);
        alertBg.setOutlineColor(sf::Color(249, 115, 22));
        alertBg.setPosition(hudX, hudY + 465.0f);
        window.draw(alertBg);

        desenharTexto(window, font, std::to_string(jogo.jogador.pontosDisponiveis) + " PONTOS!", hudX + 10.f, hudY + 472.f, 12, sf::Color::White, true);
    }

    // Divisor
    sf::RectangleShape line3(sf::Vector2f(200.0f, 1.0f));
    line3.setFillColor(sf::Color(51, 65, 85));
    line3.setPosition(hudX, hudY + 515.0f);
    window.draw(line3);

    // Resumo pontuacao
    desenharTexto(window, font, "PONTOS E PARTIDA", hudX, hudY + 530.0f, 18, sf::Color(251, 191, 36), true);
    desenharTexto(window, font, "Pontos: " + std::to_string(calcularPontuacaoFinal(jogo)), hudX, hudY + 560.0f, 15, sf::Color::White);
    desenharTexto(window, font, "Passos: " + std::to_string(jogo.pontuacao.movimentos), hudX, hudY + 580.0f, 15, sf::Color::White);
    desenharTexto(window, font, "Abates: " + std::to_string(jogo.pontuacao.inimigosDerrotados), hudX, hudY + 600.0f, 15, sf::Color::White);

    // Instrucoes basicas
    desenharTexto(window, font, "WASD : Mover", hudX, hudY + 640.0f, 13, sf::Color(148, 163, 184));
    desenharTexto(window, font, "F    : Atacar adjacente", hudX, hudY + 655.0f, 13, sf::Color(148, 163, 184));
    desenharTexto(window, font, "P    : Usar Pocao", hudX, hudY + 670.0f, 13, sf::Color(148, 163, 184));
    desenharTexto(window, font, "Q    : Abandonar", hudX, hudY + 685.0f, 13, sf::Color(239, 68, 68));


    // --- PAINEL LOG MENSAGENS (Embaixo ajustado para largura ampliada em 1920) ---
    float logX = 30.0f;
    float logY = 910.0f;
    float logW = jogo.mapa.largura * tileSize; // 41 * 40 = 1640
    float logH = 130.0f;

    // Moldura do log
    sf::RectangleShape molduraLog(sf::Vector2f(logW, logH));
    molduraLog.setFillColor(sf::Color(15, 23, 42)); // Slate-900
    molduraLog.setOutlineThickness(1.5f);
    molduraLog.setOutlineColor(sf::Color(51, 65, 85)); // Slate-700
    molduraLog.setPosition(logX, logY);
    window.draw(molduraLog);

    // Titulo do log
    sf::RectangleShape logTitleBg(sf::Vector2f(120.0f, 25.0f));
    logTitleBg.setFillColor(sf::Color(51, 65, 85));
    logTitleBg.setPosition(logX, logY);
    window.draw(logTitleBg);
    desenharTexto(window, font, "MENSAGENS", logX + 15.f, logY + 3.f, 13, sf::Color::White, true);

    // Mensagem principal grande no meio do log
    sf::Text mainMsg;
    mainMsg.setFont(font);
    mainMsg.setString(sf::String::fromUtf8(jogo.mensagem.begin(), jogo.mensagem.end()));
    mainMsg.setCharacterSize(18);
    mainMsg.setFillColor(sf::Color(253, 224, 71)); // Yellow-300
    
    // Centralizar no log
    sf::FloatRect msgBounds = mainMsg.getLocalBounds();
    mainMsg.setPosition(
        logX + (logW - msgBounds.width) / 2.f - msgBounds.left,
        logY + (logH - msgBounds.height) / 2.f - msgBounds.top
    );
    window.draw(mainMsg);

    // Dica ou contexto de status no rodape do log
    desenharTexto(window, font, "Dica: Tente interagir com NPCs (🧙) para abrir passagens e segredos.", logX + 15.f, logY + logH - 30.f, 13, sf::Color(100, 116, 139));
}

void mostrarTelaFinalSFML(sf::RenderWindow& window, const Jogo& jogo, const sf::Font& font) {
    // Fundo (Ajustado para 1920x1080)
    sf::RectangleShape bg(sf::Vector2f(1920.f, 1080.f));
    bg.setFillColor(sf::Color(15, 23, 42)); // Slate-900
    window.draw(bg);

    // Titulo principal centralizado no 1920
    std::string titulo = jogo.venceu ? "VOCE CONCLUIU A AVENTURA!" : "FIM DE JOGO";
    sf::Color tituloCor = jogo.venceu ? sf::Color(251, 191, 36) : sf::Color(239, 68, 68);
    desenharTexto(window, font, titulo, 960.f - (titulo.length() * 11.f), 100.f, 40, tituloCor, true);

    // Subtitulo descritivo
    std::string subtitulo = jogo.venceu 
        ? "Parabens! O terrivel chefe final foi derrotado!" 
        : "Sua jornada terminou nesta masmorra. Sua vida chegou a zero.";
    desenharTexto(window, font, subtitulo, 960.f - (subtitulo.length() * 4.5f), 160.f, 18, sf::Color(148, 163, 184));

    // Card de Pontos Centralizado
    float cardW = 500.f;
    float cardH = 360.f;
    float cardX = 960.f - (cardW / 2.f);
    float cardY = 280.f;

    sf::RectangleShape card(sf::Vector2f(cardW, cardH));
    card.setFillColor(sf::Color(30, 41, 59)); // Slate-800
    card.setOutlineThickness(2.0f);
    card.setOutlineColor(sf::Color(71, 85, 105)); // Slate-600
    card.setPosition(cardX, cardY);
    window.draw(card);

    // Titulo do Card
    desenharTexto(window, font, "ESTATISTICAS DE COMBATE", cardX + 30.f, cardY + 25.f, 20, sf::Color(251, 191, 36), true);

    // Stats
    float statStartX = cardX + 40.f;
    float statStartY = cardY + 75.f;
    float spacingY = 30.f;

    struct StatLine {
        std::string label;
        std::string value;
        sf::Color valueCor;
    };

    std::vector<StatLine> stats = {
        {"Pontuacao Final:", std::to_string(calcularPontuacaoFinal(jogo)), sf::Color(251, 191, 36)},
        {"Inimigos Derrotados:", std::to_string(jogo.pontuacao.inimigosDerrotados), sf::Color::White},
        {"Itens Coletados:", std::to_string(jogo.pontuacao.itensColetados), sf::Color::White},
        {"Niveis Concluidos:", std::to_string(jogo.pontuacao.niveisConcluidos), sf::Color::White},
        {"Total de Movimentos:", std::to_string(jogo.pontuacao.movimentos), sf::Color::White},
        {"Dano Recebido:", std::to_string(jogo.pontuacao.danoRecebido), sf::Color(239, 68, 68)},
        {"Pocoes Usadas:", std::to_string(jogo.pontuacao.pocoesUsadas), sf::Color::White},
        {"Vida Restante:", std::to_string(std::max(0, jogo.jogador.status.vida)), sf::Color(34, 197, 94)}
    };

    for (size_t i = 0; i < stats.size(); i++) {
        desenharTexto(window, font, stats[i].label, statStartX, statStartY + i * spacingY, 16, sf::Color(203, 213, 225));
        desenharTexto(window, font, stats[i].value, statStartX + 300.f, statStartY + i * spacingY, 16, stats[i].valueCor, true);
    }

    // Rodape
    desenharTexto(window, font, "Pressione qualquer tecla para voltar ao menu inicial...", 710.f, 700.f, 18, sf::Color(148, 163, 184));
}
