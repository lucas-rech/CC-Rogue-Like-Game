#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "entities/hero/Player.hpp"
#include "entities/enemies/Enemy.hpp"
#include "world/Level.hpp"
#include "graphics/TileMap.hpp"
#include "items/Item.hpp"
#include "traps/Trap.hpp"
#include "ui/Menu.hpp"
#include "ui/HUD.hpp"
#include "ui/GameOverScreen.hpp"
#include "ui/VictoryScreen.hpp"
#include "ui/AttributeMenu.hpp"
#include "entities/npc/NPC.hpp"
#include "entities/npc/Companion.hpp"
#include "system/Ranking.hpp"
#include "ui/RankingScreen.hpp"

using namespace sf;

struct SpellEffect {
    sf::Vector2f position;
    float age;
};

int main(int argc, char* argv[]) {
#ifdef _WIN32
    wchar_t executablePath[MAX_PATH];
    DWORD pathLength = GetModuleFileNameW(nullptr, executablePath, MAX_PATH);
    if (pathLength > 0 && pathLength < MAX_PATH) {
        for (DWORD i = pathLength; i > 0; --i) {
            if (executablePath[i - 1] == L'\\' || executablePath[i - 1] == L'/') {
                executablePath[i - 1] = L'\0';
                SetCurrentDirectoryW(executablePath);
                break;
            }
        }
    }
#else
    (void)argc;
    (void)argv;
#endif

    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow window(VideoMode(1920, 1080), "Rogue Like Game", Style::Fullscreen, settings);
    window.setFramerateLimit(60);

    Menu menu;
    HUD hud;
    GameOverScreen gameOverScreen;
    VictoryScreen victoryScreen;
    AttributeMenu attributeMenu;
    RankingScreen rankingScreen;

    if (!menu.loadFont() || !hud.loadFont() || !gameOverScreen.loadFont() || !victoryScreen.loadFont() || !attributeMenu.loadFont() || !rankingScreen.loadFont()) {
        return -1;
    }
    
    GameScreen currentScreen = GameScreen::Menu;
    bool showAttributeMenu = false;
    bool isPaused = false;

    float viewWidth = 640.f;
    float viewHeight = 360.f;

    View view(FloatRect(0, 0, viewWidth, viewHeight));
    Clock gameClock;
    float movementScoreTimer = 0.f;
    float farmTimer = 0.f;

    sf::RenderTexture fogTexture;
    bool fogReady = fogTexture.create(static_cast<unsigned int>(viewWidth), static_cast<unsigned int>(viewHeight));
    
    sf::VertexArray lightShape(sf::TriangleFan, 50);
    float radius = 120.f;
    lightShape[0].position = sf::Vector2f(0.f, 0.f);
    lightShape[0].color = sf::Color(0, 0, 0, 255);
    for (int i = 1; i <= 49; ++i) {
        float angle = (i - 1) * 2 * 3.141592654f / 48;
        lightShape[i].position = sf::Vector2f(std::cos(angle) * radius, std::sin(angle) * radius);
        lightShape[i].color = sf::Color(0, 0, 0, 0);
    }

    Player player = Player(20 * 16.0f, 10 * 16.0f);
    int currentVisualLevel = 1;

    auto loadPlayerVisuals = [&]() {
        currentVisualLevel = std::min(player.getLevel(), 3);
        std::string classStr = "warrior";
        std::string basePath = "assets/textures/characters/player/"
                             + classStr + "/" + classStr + "_lvl"
                             + std::to_string(currentVisualLevel) + "/Without_shadow/";

        return player.loadTextures(basePath + "idle.png",
                                   basePath + "walk.png",
                                   basePath + "attack.png",
                                   basePath + "hurt.png",
                                   basePath + "death.png");
    };

    if (!loadPlayerVisuals()) {
        return -1;
    }

    sf::SoundBuffer swordBuffer;
    sf::SoundBuffer spellBuffer;
    sf::SoundBuffer batBuffer;
    bool swordSoundLoaded = swordBuffer.loadFromFile("assets/sfx/blade.mp3");
    bool spellSoundLoaded = spellBuffer.loadFromFile("assets/sfx/light_spell.mp3");
    bool batSoundLoaded = batBuffer.loadFromFile("assets/sfx/bat.mp3");
    sf::Sound swordSound(swordBuffer);
    sf::Sound spellSound(spellBuffer);
    sf::Sound batSound(batBuffer);

    std::vector<std::string> songs = {
        "src/assets/sfx/songs/the_king.wav",
        "src/assets/sfx/songs/rituals.wav",
        "src/assets/sfx/songs/witche_enchatment.wav"
    };
    int currentSong = 0;
    sf::Music backgroundMusic;

    auto playSong = [&]() {
        std::string path = songs[currentSong];
        std::string localPath = path.substr(4); // Remove "src/"
        if (!backgroundMusic.openFromFile(localPath)) {
            if (!backgroundMusic.openFromFile(path)) {
                std::cout << "[AUDIO] Erro ao carregar: " << path << "\n";
            }
        }
        backgroundMusic.setLoop(false); // We want to cycle manually
        backgroundMusic.setVolume(30.f);
        backgroundMusic.play();
        std::cout << "[AUDIO] Tocando agora: " << path << "\n";
    };

    playSong();

    std::vector<sf::Texture> spellTextures;
    spellTextures.reserve(10);
    for (int i = 1; i <= 10; ++i) {
        sf::Texture texture;
        if (texture.loadFromFile("assets/textures/characters/player/vampire/magic/Explosion_blue_circle" + std::to_string(i) + ".png")) {
            spellTextures.push_back(std::move(texture));
        }
    }
    std::vector<SpellEffect> spellEffects;

    GameState gameState = {0};
    gameState.maxLevel = 1;
    loadLevel(gameState, 1, player);
    gameState.stats.updateScore(gameState.currentLevel);

    std::vector<Enemy> enemies;
    spawnEnemies(enemies, gameState, player);

    std::vector<Item> items;
    spawnItems(items, gameState, player);

    std::vector<Trap> traps;
    spawnTraps(traps, gameState, player);

    auto reloadStageContent = [&]() {
        spawnEnemies(enemies, gameState, player);
        spawnItems(items, gameState, player);
        spawnTraps(traps, gameState, player);
        spellEffects.clear();
        movementScoreTimer = 0.f;
    };

    std::vector<NPC> npcs;
    npcs.reserve(10); // Evitar realocacao que quebra o ponteiro de fonte do sf::Text
    Companion companion(player.getPosition().x, player.getPosition().y);
    companion.loadTexture();
    npcs.emplace_back(sf::Vector2f(2440.55f, 1508.95f));
    npcs[0].setFrameProperties(6, 64, 64);
    npcs[0].loadTexture("assets/textures/characters/slime/Without_shadow/idle.png");
    npcs[0].setDialog("Ha muito tempo... esta grande floresta guardava a verdadeira fonte\nde magia. Mas uma sombra caiu sobre nos.");

    npcs.emplace_back(sf::Vector2f(1712.6f, 106.45f));
    npcs[1].setFrameProperties(6, 64, 64);
    npcs[1].loadTexture("assets/textures/characters/slime/Without_shadow/idle.png");
    npcs[1].setDialog("Cuidado, guerreiro... O antigo Lorde Vampiresco ainda habita\no fundo desta floresta. Ele consome os fracos!");

    npcs.emplace_back(sf::Vector2f(2628.5f, 860.499f));
    npcs[2].setFrameProperties(6, 64, 64);
    npcs[2].loadTexture("assets/textures/characters/slime/Without_shadow/idle.png");
    npcs[2].setDialog("Parabens por chegar ate aqui, aventureiro! Voce encontrou o castelo ancestral.\nMas para derrotar o vampiro e libertar a floresta, voce precisara\nreunir todas as chaves e destrancar o portao principal adiante!");

    npcs.emplace_back(sf::Vector2f(417.5f, 569.5f));
    npcs[3].setFrameProperties(6, 64, 64);
    npcs[3].loadTexture("assets/textures/characters/slime/Without_shadow/idle.png");
    npcs[3].setDialog("Outrora o Conde era um protetor desta terra pacifica...\nMas sua obsessao pelo poder o levou a desenterrar segredos antigos.");

    npcs.emplace_back(sf::Vector2f(1500.0f, 1200.0f));
    npcs[4].setFrameProperties(6, 64, 64);
    npcs[4].loadTexture("assets/textures/characters/slime/Without_shadow/idle.png");
    npcs[4].setDialog("Ao abrir a cripta proibida, o Conde liberou uma magia obscura.\nEssa maldicao consumiu sua alma e agora escurece toda a vida da floresta...");

    npcs.emplace_back(sf::Vector2f(400.0f, 1800.0f));
    npcs[5].setFrameProperties(6, 64, 64);
    npcs[5].loadTexture("assets/textures/characters/slime/Without_shadow/idle.png");
    npcs[5].setDialog("A nevoa esta em rapida expansao, pronta para corroer o mundo inteiro.\nSomente voce pode impedi-lo antes que seja tarde demais. Salve-nos!");

    npcs.emplace_back(sf::Vector2f(776.15f, 962.65f));
    npcs[6].setFrameProperties(6, 64, 64);
    npcs[6].loadTexture("assets/textures/characters/slime/Without_shadow/idle.png");
    npcs[6].setDialog("Vejo que encontrou o Livro Antigo...\nAperte Espaco para usa-lo. Ele consome Mana, mas e devastador!");

    auto finalizeCampaign = [&](bool victory) {
        if (!gameState.finalScoreCalculated) {
            gameState.finalScore = calculateFinalCampaignScore(gameState, player.getCurrentHp(), victory);
            gameState.finalScoreCalculated = true;
            Ranking::saveScore(gameState);
        }
        gameState.campaignActive = false;
    };

    auto resetGame = [&](Difficulty difficulty, const std::string& playerName, bool autoPlay) {
        player.resetForNewGame(20.f * 16.f, 10.f * 16.f);
        loadPlayerVisuals();
        gameState = GameState{};
        gameState.isAutoPlay = autoPlay;
        gameState.difficulty = difficulty;
        gameState.playerName = playerName.empty() ? "Jogador" : playerName;
        gameState.campaignScore = 0;
        gameState.completedLevels = 0;
        gameState.maxLevel = 1;
        gameState.campaignElapsedTime = 0.f;
        gameState.campaignActive = true;
        gameState.finalScoreCalculated = false;
        loadLevel(gameState, 1, player);
        gameState.isGameOver = false;
        gameState.isVictory = false;
        gameState.stats.updateScore(gameState.currentLevel);
        companion.setPosition(player.getPosition().x, player.getPosition().y);
        reloadStageContent();
        currentScreen = GameScreen::Playing;
        gameClock.restart();
    };

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (currentScreen == GameScreen::Menu ||
                currentScreen == GameScreen::PlayerName ||
                currentScreen == GameScreen::DifficultySelect ||
                currentScreen == GameScreen::HowToPlay ||
                currentScreen == GameScreen::ItemsInfo ||
                currentScreen == GameScreen::ScoreInfo ||
                currentScreen == GameScreen::Ranking) {
                
                GameScreen nextScreen = menu.handleEvent(event, currentScreen);
                if (nextScreen == GameScreen::Exit) {
                    window.close();
                } else if (nextScreen == GameScreen::Playing) {
                    bool autoPlay = (menu.getPlayerName() == "AutoPlay-AI");
                    resetGame(menu.getSelectedDifficulty(), menu.getPlayerName(), autoPlay);
                } else {
                    currentScreen = nextScreen;
                }
            } else if (currentScreen == GameScreen::Playing) {
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    currentScreen = GameScreen::Menu;
                } else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Q) {
                    if (player.usePotion()) {
                        gameState.stats.registerPotionUsed(gameState.currentLevel);
                    }
                } else if (event.type == Event::KeyPressed && event.key.code == Keyboard::M) {
                    if (player.useManaPotion()) {
                        gameState.stats.registerPotionUsed(gameState.currentLevel);
                    }
                } else if (event.type == Event::KeyPressed && event.key.code == Keyboard::E) {
                    // Verificacao para abrir o portao do castelo
                    sf::Vector2f pPos = player.getCenterPosition();
                    if (!gameState.isGateOpen && pPos.y > 658.f && pPos.y < 750.f && pPos.x >= 2400.f && pPos.x <= 2700.f) {
                        if (player.getKeys() >= 3) {
                            if (gameState.currentLevel < gameState.maxLevel) {
                                const int nextLevel = gameState.currentLevel + 1;
                                gameState.campaignScore += 500;
                                gameState.completedLevels++;
                                player.clearKeys();
                                loadLevel(gameState, nextLevel, player);
                                gameState.stats.updateScore(gameState.currentLevel);
                                reloadStageContent();
                                gameClock.restart();
                                std::cout << "Avancando para a fase " << gameState.currentLevel << ".\n";
                            } else {
                                gameState.isGateOpen = true;
                                player.setPosition(pPos.x - 32.f, 450.f);
                                std::cout << "O Portao do Castelo foi aberto!\n";
                            }
                        } else {
                            std::cout << "Voce precisa de todas as 3 chaves para abrir o portao.\n";
                        }
                    }
                } else if (event.type == Event::KeyPressed && event.key.code == Keyboard::I) {
                    showAttributeMenu = !showAttributeMenu;
                } else if (event.type == Event::KeyPressed && event.key.code == Keyboard::P) {
                    isPaused = !isPaused;
                } else if (event.type == Event::KeyPressed && showAttributeMenu) {
                    if (event.key.code == Keyboard::Num1) {
                        player.upgradeHealth();
                    } else if (event.key.code == Keyboard::Num2) {
                        player.upgradeDamage();
                    } else if (event.key.code == Keyboard::Num3) {
                        player.upgradeSpeed();
                    } else if (event.key.code == Keyboard::Escape) {
                        showAttributeMenu = false;
                    }
                }
            } else if (currentScreen == GameScreen::GameOver || currentScreen == GameScreen::Victory) {
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                    resetGame(gameState.difficulty, gameState.playerName, gameState.isAutoPlay);
                } else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    currentScreen = GameScreen::Menu;
                } else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Q) {
                    window.close();
                }
            }
        }

        if (currentScreen == GameScreen::GameOver) {
            gameClock.restart();
            window.clear();
            gameOverScreen.draw(window, player, gameState);
            window.display();
            continue;
        }

        if (currentScreen == GameScreen::Victory) {
            gameClock.restart();
            window.clear();
            victoryScreen.draw(window, player, gameState);
            window.display();
            continue;
        }

        if (currentScreen == GameScreen::Playing && backgroundMusic.getStatus() == sf::Music::Stopped) {
            currentSong = (currentSong + 1) % songs.size();
            std::cout << "[AUDIO] Musica parou. Trocando para o indice: " << currentSong << "\n";
            playSong();
        } else if (currentScreen == GameScreen::Ranking) {
            rankingScreen.draw(window);
            window.display();
            continue;
        }

        if (currentScreen != GameScreen::Playing) {
            gameClock.restart();
            window.clear();
            window.setView(window.getDefaultView());
            menu.draw(window, currentScreen);
            window.display();
            continue;
        }

        if (isPaused) {
            window.clear();
            window.setView(view);

            gameState.map.drawAll(window, RenderStates::Default);
            drawItems(window, items);
            drawTraps(window, traps);
            drawEnemies(window, enemies);
            for (auto& npc : npcs) window.draw(npc);
            window.draw(companion);
            window.draw(player);
            gameState.map.drawForeground(window, RenderStates::Default);

            sf::Text pauseText = hud.makeText("PAUSADO", view.getCenter().x - 40.f, view.getCenter().y - 20.f);
            pauseText.setCharacterSize(36);
            pauseText.setFillColor(sf::Color::Yellow);
            window.draw(pauseText);

            hud.draw(window, player, gameState);
            window.display();
            continue; // Skip all updates!
        }

        if (showAttributeMenu) {
            gameClock.restart();
            window.clear();
            
            // Desenha o jogo por baixo (pausado)
            sf::View view(player.getCenterPosition(), sf::Vector2f(viewWidth, viewHeight));
            window.setView(view);
            window.draw(gameState.map);
            drawTraps(window, traps);
            drawItems(window, items);
            for (const Enemy& enemy : enemies) {
                if (enemy.isAlive() || !enemy.isDeadComplete()) window.draw(enemy);
            }
            window.draw(player);
            window.draw(companion);
            for (const NPC& npc : npcs) {
                window.draw(npc);
            }
            hud.draw(window, player, gameState);

            // Desenha o menu por cima
            attributeMenu.draw(window, player);

            window.display();
            continue;
        }

        float deltaTime = gameClock.restart().asSeconds();
        if (gameState.campaignActive && !showAttributeMenu) {
            gameState.campaignElapsedTime += deltaTime;
        }
        movementScoreTimer += deltaTime;
        if (movementScoreTimer >= 5.0f) {
            gameState.stats.registerMovement(gameState.currentLevel);
            movementScoreTimer = 0.f;
        }

        farmTimer += deltaTime;
        if (farmTimer >= 30.f) {
            farmTimer = 0.f;
            spawnRandomEnemy(enemies, gameState, player);
        }

        if (gameState.isAutoPlay) {
            player.autoPlayLogic(deltaTime, gameState, items, enemies);
        } else {
            player.processInput(window);
        }

        FloatRect intendedHitBox = player.getNextHitbox();
        bool canMove = !checkCollision(intendedHitBox, gameState);

        if (player.updateAndMove(canMove, deltaTime)) {
            sf::Vector2f pos = player.getPosition();
            std::cout << "Coordenadas do personagem: X=" << pos.x << ", Y=" << pos.y << std::endl;
        }

        int newVisualLevel = std::min(player.getLevel(), 3);
        if (newVisualLevel != currentVisualLevel) {
            loadPlayerVisuals();
        }

        bool playerAttacked = player.hasPendingAttack();
        if (playerAttacked) {
            if (!player.isPendingAttackMagic()) {
                if (swordSoundLoaded) swordSound.play();
            } else {
                if (spellSoundLoaded) spellSound.play();
                if (!spellTextures.empty()) {
                    spellEffects.push_back({player.getTargetPos(), 0.f});
                }
            }
        }

        handlePlayerAttack(enemies, player, gameState);
        updateEnemies(enemies, player, gameState, deltaTime);

        for (Enemy& enemy : enemies) {
            if (enemy.isAlive()) {
                float minDistance = 400.f;
                sf::Vector2f targetItemPos(0, 0);

                for (Item& item : items) {
                    if (!item.isCollected() && item.getType() != ItemType::Key && item.getType() != ItemType::PowerUp && item.getType() != ItemType::SpellTome) {
                        sf::FloatRect bounds = item.getBounds();
                        sf::Vector2f itemPos(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
                        sf::Vector2f toItem = itemPos - enemy.getCenterPosition();
                        float dist = std::sqrt(toItem.x * toItem.x + toItem.y * toItem.y);

                        if (dist < minDistance) {
                            minDistance = dist;
                            targetItemPos = itemPos;
                        }
                    }
                }
                enemy.setTargetItemPos(targetItemPos);
            }
        }

        updateItems(items, player, gameState, enemies);
        updateTraps(traps, player, gameState, deltaTime);
        companion.update(deltaTime, player, items);
        for (auto& npc : npcs) {
            npc.update(deltaTime, player);
        }

        // Bloqueio definitivo na arena do boss
        if (gameState.isGateOpen && !gameState.playerInArena) {
            if (player.getCenterPosition().y < 640.f && player.getCenterPosition().x >= 2400.f && player.getCenterPosition().x <= 2800.f) {
                gameState.playerInArena = true;
                std::cout << "O portao se fechou atras de voce! Nao ha volta!\n";
            }
        }

        for (SpellEffect& effect : spellEffects) {
            effect.age += deltaTime;
        }
        spellEffects.erase(
            std::remove_if(spellEffects.begin(), spellEffects.end(), [](const SpellEffect& effect) {
                return effect.age > 0.6f;
            }),
            spellEffects.end());

        if (batSoundLoaded && player.getCurrentHp() <= 0 && !gameState.isGameOver) {
            batSound.play();
        }

        if (player.getCurrentHp() <= 0 || gameState.isGameOver || gameState.isVictory) {
            if (gameState.isGameOver || player.getCurrentHp() <= 0) {
                finalizeCampaign(false);
                currentScreen = GameScreen::GameOver;
            } else if (gameState.isVictory) {
                finalizeCampaign(true);
                currentScreen = GameScreen::Victory;
            }
            continue;
        }

        sf::Vector2f cameraPos = player.getCenterPosition();

        float viewW = view.getSize().x;
        float viewH = view.getSize().y;
        float mapW = gameState.map.getWidth() * gameState.map.getTileSize();
        float mapH = gameState.map.getHeight() * gameState.map.getTileSize();

        if (mapW < viewW) {
            cameraPos.x = mapW / 2.0f;
        } else {
            if (cameraPos.x < viewW / 2.0f) cameraPos.x = viewW / 2.0f;
            else if (cameraPos.x > mapW - viewW / 2.0f) cameraPos.x = mapW - viewW / 2.0f;
        }

        if (mapH < viewH) {
            cameraPos.y = mapH / 2.0f;
        } else {
            if (cameraPos.y < viewH / 2.0f) cameraPos.y = viewH / 2.0f;
            else if (cameraPos.y > mapH - viewH / 2.0f) cameraPos.y = mapH - viewH / 2.0f;
        }

        view.setCenter(cameraPos);

        window.clear();
        window.setView(view);

        gameState.map.drawAll(window, RenderStates::Default);

        drawItems(window, items);
        drawTraps(window, traps);
        drawEnemies(window, enemies);

        for (const SpellEffect& effect : spellEffects) {
            std::size_t frame = std::min<std::size_t>(
                static_cast<std::size_t>(effect.age / 0.06f),
                spellTextures.size() - 1);
            sf::Sprite spellSprite(spellTextures[frame]);
            sf::FloatRect bounds = spellSprite.getLocalBounds();
            spellSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            spellSprite.setPosition(effect.position);
            window.draw(spellSprite);
        }

        for (auto& npc : npcs) {
            window.draw(npc);
        }
        window.draw(companion);
        window.draw(player);

        gameState.map.drawForeground(window, RenderStates::Default);

        // Desenhar dialogos sobrescrevendo as texturas (abaixo apenas dos filtros)
        for (auto& npc : npcs) {
            npc.drawDialog(window, RenderStates::Default);
        }
        player.drawDialog(window, RenderStates::Default);

        sf::RectangleShape nightFilter(sf::Vector2f(viewWidth, viewHeight));
        nightFilter.setFillColor(sf::Color(10, 16, 32, 85));
        nightFilter.setPosition(view.getCenter().x - viewWidth / 2.f, view.getCenter().y - viewHeight / 2.f);
        window.draw(nightFilter);

        if (fogReady) {
            fogTexture.setView(fogTexture.getDefaultView());
            fogTexture.clear(sf::Color(0, 0, 0, 210));

            sf::Vector2f viewTopLeft(view.getCenter().x - viewWidth / 2.f,
                                     view.getCenter().y - viewHeight / 2.f);
            sf::Vector2f playerScreenPos = player.getCenterPosition() - viewTopLeft;

            sf::BlendMode revealMode(sf::BlendMode::Zero,
                                     sf::BlendMode::OneMinusSrcAlpha,
                                     sf::BlendMode::Add);
            sf::RenderStates states;
            states.blendMode = revealMode;
            sf::Transform transform;
            transform.translate(playerScreenPos);
            states.transform = transform;
            fogTexture.draw(lightShape, states);
            fogTexture.display();

            sf::Sprite fogSprite(fogTexture.getTexture());
            fogSprite.setPosition(viewTopLeft);
            window.draw(fogSprite);
        }

        hud.draw(window, player, gameState);

        window.display();
    }

    return 0;
}
