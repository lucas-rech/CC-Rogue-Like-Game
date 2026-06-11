#include "game.h"
#include "input.h"
#include "map.h"
#include "menu.h"
#include "render.h"
#include "utils.h"

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

bool carregarTextura(sf::Texture& textura, const std::string& nomeArquivo) {
    if (textura.loadFromFile("src/assets/" + nomeArquivo)) {
        return true;
    }
    if (textura.loadFromFile("assets/" + nomeArquivo)) {
        return true;
    }
    if (textura.loadFromFile(nomeArquivo)) {
        return true;
    }
    return false;
}

int main() {
    inicializarAleatorio();

    // 1. Inicializa a janela grafica do jogo SFML (Largura: 1920, Altura: 1080)
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "CC Rogue-Like Dungeon - SFML Edition", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // 2. Tenta carregar uma fonte padrao do Windows
    sf::Font font;
    bool fonteCarregada = false;
    std::vector<std::string> caminhosFontes = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/consolas.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/cour.ttf",
        "arial.ttf"
    };

    for (const auto& caminho : caminhosFontes) {
        if (font.loadFromFile(caminho)) {
            fonteCarregada = true;
            break;
        }
    }

    if (!fonteCarregada) {
        std::cerr << "Erro: Nao foi possivel carregar nenhuma fonte de texto do sistema!\n";
        return -1;
    }

    // 2.5. Inicializa as texturas de Orcs e do Heroi (Idle e Ataque)
    TexturasJogo texturas;
    texturas.jogadorCarregado = carregarTextura(texturas.jogador, "enemies/Orc1/Without_shadow/orc1_idle_without_shadow.png");
    texturas.comumCarregado = carregarTextura(texturas.inimigoComum, "enemies/Orc2/Without_shadow/orc2_idle_without_shadow.png");
    texturas.perseguidorCarregado = carregarTextura(texturas.inimigoPerseguidor, "enemies/Orc3/Without_shadow/orc3_idle_without_shadow.png");
    texturas.tanqueCarregado = carregarTextura(texturas.inimigoTanque, "enemies/Orc3/Without_shadow/orc3_idle_without_shadow.png");
    texturas.bossCarregado = carregarTextura(texturas.boss, "enemies/Orc3/Without_shadow/orc3_idle_without_shadow.png");

    texturas.jogadorAtaqueCarregado = carregarTextura(texturas.jogadorAtaque, "enemies/Orc1/Without_shadow/orc1_attack_without_shadow.png");
    texturas.comumAtaqueCarregado = carregarTextura(texturas.comumAtaque, "enemies/Orc2/Without_shadow/orc2_attack_without_shadow.png");
    texturas.perseguidorAtaqueCarregado = carregarTextura(texturas.perseguidorAtaque, "enemies/Orc3/Without_shadow/orc3_attack_without_shadow.png");
    texturas.tanqueAtaqueCarregado = carregarTextura(texturas.tanqueAtaque, "enemies/Orc3/Without_shadow/orc3_attack_without_shadow.png");
    texturas.bossAtaqueCarregado = carregarTextura(texturas.bossAtaque, "enemies/Orc3/Without_shadow/orc3_attack_without_shadow.png");

    // 3. Inicializa o estado do jogo
    Jogo jogo;
    jogo.estado = EstadoJogo::MENU;
    jogo.rodando = false;

    sf::Clock frameClock;

    // --- GAME LOOP ---
    while (window.isOpen()) {
        float dt = frameClock.restart().asSeconds();

        // Atualiza temporizadores de animacao de ataque
        if (jogo.estado == EstadoJogo::JOGANDO) {
            if (jogo.jogador.tempoAtaque > 0.0f) {
                jogo.jogador.tempoAtaque -= dt;
            }
            for (auto& inimigo : jogo.mapa.inimigos) {
                if (inimigo.tempoAtaque > 0.0f) {
                    inimigo.tempoAtaque -= dt;
                }
            }
        }
        
        // --- 1. PROCESSAR ENTRADAS / EVENTOS ---
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                // Eventos dependendo do Estado do Jogo
                if (jogo.estado == EstadoJogo::MENU) {
                    if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
                        inicializarJogo(jogo);
                        jogo.estado = EstadoJogo::JOGANDO;
                    } else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
                        jogo.estado = EstadoJogo::COMO_JOGAR;
                    } else if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) {
                        jogo.estado = EstadoJogo::EXPLICACAO_ITENS;
                    } else if (event.key.code == sf::Keyboard::Num4 || event.key.code == sf::Keyboard::Numpad4) {
                        jogo.estado = EstadoJogo::EXPLICACAO_PONTUACAO;
                    } else if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                }
                else if (jogo.estado == EstadoJogo::COMO_JOGAR || 
                         jogo.estado == EstadoJogo::EXPLICACAO_ITENS || 
                         jogo.estado == EstadoJogo::EXPLICACAO_PONTUACAO) {
                    // Qualquer tecla pressionada retorna ao menu principal
                    jogo.estado = EstadoJogo::MENU;
                }
                else if (jogo.estado == EstadoJogo::JOGANDO) {
                    char tecla = mapearTeclaSFML(event.key.code);
                    if (tecla != 0) {
                        processarEntrada(jogo, tecla);
                        
                        // Se o jogador usou comando de sair/morreu
                        if (!jogo.rodando) {
                            jogo.estado = EstadoJogo::TELA_FINAL;
                        }
                    }
                }
                else if (jogo.estado == EstadoJogo::TELA_FINAL) {
                    // Qualquer tecla pressionada retorna ao menu
                    jogo.estado = EstadoJogo::MENU;
                }
            }
        }

        // --- 2. ATUALIZAÇÃO DO JOGO EM TEMPO REAL ---
        if (jogo.estado == EstadoJogo::JOGANDO && jogo.rodando) {
            float dx = 0.f;
            float dy = 0.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dy -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dy += 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dx -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dx += 1.f;

            // Normaliza o vetor de direção do movimento
            float len = std::sqrt(dx*dx + dy*dy);
            if (len > 0.f) {
                dx /= len;
                dy /= len;
            }

            atualizarJogoRealTime(jogo, dt, dx, dy);

            if (!jogo.rodando) {
                jogo.estado = EstadoJogo::TELA_FINAL;
            }
        }

        // --- 3. RENDERIZAR NA TELA ---
        window.clear();

        if (jogo.estado == EstadoJogo::MENU) {
            renderizarMenuSFML(window, font);
        } else if (jogo.estado == EstadoJogo::COMO_JOGAR) {
            renderizarComoJogarSFML(window, font);
        } else if (jogo.estado == EstadoJogo::EXPLICACAO_ITENS) {
            renderizarExplicacaoItensSFML(window, font);
        } else if (jogo.estado == EstadoJogo::EXPLICACAO_PONTUACAO) {
            renderizarExplicacaoPontuacaoSFML(window, font);
        } else if (jogo.estado == EstadoJogo::JOGANDO) {
            renderizarJogoSFML(window, jogo, font, texturas);
        } else if (jogo.estado == EstadoJogo::TELA_FINAL) {
            mostrarTelaFinalSFML(window, jogo, font);
        }

        window.display();
    }

    return 0;
}
