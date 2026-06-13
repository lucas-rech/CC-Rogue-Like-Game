#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#include "entities/hero/Player.hpp"
#include "entities/enemies/Enemy.hpp"
#include "world/Level.hpp"
#include "graphics/TileMap.hpp"

struct MagicEffect {
    sf::Vector2f position;
    int currentFrame = 0;
    sf::Clock timer;
    bool isFinished = false;
};

using namespace sf;

int main() {
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow window(VideoMode(1920, 1080), "Rogue Like Game", Style::Fullscreen, settings);
    window.setFramerateLimit(60);


    float viewWidth = 640.f;
    float viewHeight = 360.f;

    View view(FloatRect(0, 0, viewWidth, viewHeight));


    // --- 2. CONFIGURAÇÕES DO JOGADOR E MAGIA ---
    Player player = Player(HeroClass::Vampire, 20 * 16.0f, 10 * 16.0f);
    int currentVisualLevel = std::min(player.getLevel(), 3);

    std::vector<Texture> magicTextures;
    for (int i = 1; i <= 10; i++) {
        Texture tex;
        if (tex.loadFromFile("assets/textures/characters/player/vampire/magic/Explosion_blue_circle" + std::to_string(i) + ".png")) {
            magicTextures.push_back(tex);
        }
    }
    std::vector<MagicEffect> activeMagicEffects;

    sf::SoundBuffer swordBuffer;
    if (!swordBuffer.loadFromFile("assets/sfx/blade.mp3")) {
        std::cout << "Erro ao carregar o som da espada!" << std::endl;
    }
    sf::Sound swordSound(swordBuffer);

    sf::SoundBuffer spellBuffer;
    if (!spellBuffer.loadFromFile("assets/sfx/light_spell.mp3")) {
        std::cout << "Erro ao carregar o som da magia!" << std::endl;
    }
    sf::Sound spellSound(spellBuffer);

    sf::SoundBuffer batBuffer;
    if (!batBuffer.loadFromFile("assets/sfx/bat.mp3")) {
        std::cout << "Erro ao carregar o som do orc!" << std::endl;
    }
    sf::Sound batSound(batBuffer);

    switch(player.getClass()) {
        case(HeroClass::Warrior):
            if (!player.loadTextures(
            "assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/idle.png",
            "assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/walk.png",
            "assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/attack.png",
            "assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/hurt.png",
            "assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/death.png")) {
                return -1;
            }
            break;
        case(HeroClass::Vampire):
            if (!player.loadTextures(
                "assets/textures/characters/player/vampire/vampire_lvl1/Without_shadow/idle.png",
                "assets/textures/characters/player/vampire/vampire_lvl1/Without_shadow/walk.png",
                "assets/textures/characters/player/vampire/vampire_lvl1/Without_shadow/attack.png",
                "assets/textures/characters/player/vampire/vampire_lvl1/Without_shadow/hurt.png",
                "assets/textures/characters/player/vampire/vampire_lvl1/Without_shadow/death.png")) {
                return -1;
            }
    }


    // --- CONFIGURAÇÕES DO MAPA ---
    GameState gameState = {0};
    loadLevel(gameState, 1, player);

    // --- CONFIGURAÇÕES DOS INIMIGOS ---
    std::vector<Enemy> enemies;
    enemies.emplace_back(EnemyType::Orc1, 15 * 16.0f, 15 * 16.0f);
    enemies.emplace_back(EnemyType::Orc2, 25 * 16.0f, 18 * 16.0f);
    enemies.emplace_back(EnemyType::Orc3, 30 * 16.0f, 10 * 16.0f);

    for (auto& enemy : enemies) {
        enemy.loadTextures(
            "assets/textures/characters/enemies/Orc1/Without_shadow/idle.png",
            "assets/textures/characters/enemies/Orc1/Without_shadow/walk.png",
            "assets/textures/characters/enemies/Orc1/Without_shadow/attack.png",
            "assets/textures/characters/enemies/Orc1/Without_shadow/hurt.png",
            "assets/textures/characters/enemies/Orc1/Without_shadow/death.png"
        );
    }

    // --- LOOP PRINCIPAL ---
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) window.close();
        }

        // --- ENTRADA DE MOVIMENTAÇÃO ---
        player.processInput(window);

        FloatRect intendedHitBox = player.getNextHitbox();
        bool canMove = !checkCollision(intendedHitBox, gameState);
        player.updateAndMove(canMove);

        // --- ATUALIZAÇÃO VISUAL POR LEVEL ---
        if (player.getLevel() > currentVisualLevel && currentVisualLevel < 3) {
            currentVisualLevel = std::min(player.getLevel(), 3);
            std::string classStr = player.getClass() == HeroClass::Warrior ? "warrior" : "vampire";
            std::string basePath = "assets/textures/characters/player/" + classStr + "/" + classStr + "_lvl" + std::to_string(currentVisualLevel) + "/Without_shadow/";
            player.loadTextures(basePath + "idle.png", basePath + "walk.png", basePath + "attack.png", basePath + "hurt.png", basePath + "death.png");
        }

        // --- LÓGICA DE ATAQUE DO PLAYER ---
        if (player.popAttackFlag()) {
            sf::Vector2f pCenter = player.getCenterPosition();
            float pRange = player.getAttackRange();

            if (player.getClass() == HeroClass::Warrior) {
                swordSound.play();
                sf::Vector2f aimDir = player.getAimDirection();
                for (auto& enemy : enemies) {
                    if (enemy.getIsDead()) continue;
                    sf::Vector2f eCenter = enemy.getCenterPosition();
                    float dx = eCenter.x - pCenter.x;
                    float dy = eCenter.y - pCenter.y;
                    float dist = std::sqrt(dx*dx + dy*dy);

                    if (dist <= pRange) {
                        sf::Vector2f dirToEnemy(dx/dist, dy/dist);
                        float dotProduct = (dirToEnemy.x * aimDir.x) + (dirToEnemy.y * aimDir.y);
                        if (dotProduct > 0.5f) { // ~90 graus de cone
                            bool wasDead = enemy.getIsDead();
                            enemy.takeDamage(player.getBaseDamage());
                            if (!wasDead && enemy.getIsDead()) {
                                player.gainExp(65); // ganha exp
                            }
                        }
                    }
                }
            } else if (player.getClass() == HeroClass::Vampire) {
                spellSound.play();
                sf::Vector2f targetPos = player.getTargetPos();
                float dx = targetPos.x - pCenter.x;
                float dy = targetPos.y - pCenter.y;
                float distToTarget = std::sqrt(dx*dx + dy*dy);
                
                // Clampar a distância máxima do feitiço
                if (distToTarget > pRange) {
                    targetPos.x = pCenter.x + (dx/distToTarget) * pRange;
                    targetPos.y = pCenter.y + (dy/distToTarget) * pRange;
                }

                float aoeRadius = 60.f; // Raio de dano da magia

                // Instancia o efeito
                activeMagicEffects.push_back({targetPos, 0, sf::Clock(), false});

                // Aplica dano em área
                for (auto& enemy : enemies) {
                    if (enemy.getIsDead()) continue;
                    sf::Vector2f eCenter = enemy.getCenterPosition();
                    float ex = eCenter.x - targetPos.x;
                    float ey = eCenter.y - targetPos.y;
                    float eDist = std::sqrt(ex*ex + ey*ey);

                    if (eDist <= aoeRadius) {
                        bool wasDead = enemy.getIsDead();
                        enemy.takeDamage(player.getBaseDamage());
                        if (!wasDead && enemy.getIsDead()) {
                            player.gainExp(35); // Ganha EXP ao matar
                        }
                    }
                }
            }
        }

        // --- ATUALIZAÇÃO DOS INIMIGOS E SEPARAÇÃO ---
        for (size_t i = 0; i < enemies.size(); i++) {
            if (enemies[i].getIsDead()) {
                enemies[i].updateAndMove(player.getCenterPosition(), true);
                continue;
            }

            sf::Vector2f push(0.f, 0.f);
            for (size_t j = 0; j < enemies.size(); j++) {
                if (i == j || enemies[j].getIsDead()) continue;
                sf::Vector2f p1 = enemies[i].getCenterPosition();
                sf::Vector2f p2 = enemies[j].getCenterPosition();
                float dx = p1.x - p2.x;
                float dy = p1.y - p2.y;
                float dist = std::sqrt(dx*dx + dy*dy);
                if (dist > 0.01f && dist < 45.f) { // Se sobrepondo
                     push.x += (dx/dist) * 1.5f;
                     push.y += (dy/dist) * 1.5f;
                }
            }

            enemies[i].updateAndMove(player.getCenterPosition(), true, push);
            
            if (enemies[i].popAttackFlag()) {
                batSound.play();
                
                if (!player.checkDead()) {
                    sf::Vector2f eCenter = enemies[i].getCenterPosition();
                    sf::Vector2f pCenter = player.getCenterPosition();
                    float dx = pCenter.x - eCenter.x;
                    float dy = pCenter.y - eCenter.y;
                    float dist = std::sqrt(dx*dx + dy*dy);

                    if (dist <= enemies[i].getAttackRange() + 20.f) { // margem
                        player.takeDamage(enemies[i].getBaseDamage());
                    }
                }
            }
        }

        // --- ATUALIZAÇÃO DE MAGIA ---
        for (auto& effect : activeMagicEffects) {
            if (effect.timer.getElapsedTime().asSeconds() > 0.05f) {
                effect.currentFrame++;
                effect.timer.restart();
                if (effect.currentFrame >= magicTextures.size()) {
                    effect.isFinished = true;
                }
            }
        }
        activeMagicEffects.erase(std::remove_if(activeMagicEffects.begin(), activeMagicEffects.end(), 
            [](const MagicEffect& e) { return e.isFinished; }), activeMagicEffects.end());


        // --- ATUALIZAÇÃO DA CÂMERA ---
        sf::Vector2f cameraPos = player.getCenterPosition();
        
        // Limites da câmera para ela não sair das bordas do mapa
        float viewW = view.getSize().x;
        float viewH = view.getSize().y;
        float mapW = gameState.map.getWidth() * gameState.map.getTileSize();
        float mapH = gameState.map.getHeight() * gameState.map.getTileSize();

        // Trava no eixo X
        if (mapW < viewW) {
            cameraPos.x = mapW / 2.0f; // Mapa menor que a tela, centraliza à força
        } else {
            if (cameraPos.x < viewW / 2.0f) cameraPos.x = viewW / 2.0f;
            else if (cameraPos.x > mapW - viewW / 2.0f) cameraPos.x = mapW - viewW / 2.0f;
        }

        // Trava no eixo Y
        if (mapH < viewH) {
            cameraPos.y = mapH / 2.0f; // Mapa menor que a tela, centraliza à força
        } else {
            if (cameraPos.y < viewH / 2.0f) cameraPos.y = viewH / 2.0f;
            else if (cameraPos.y > mapH - viewH / 2.0f) cameraPos.y = mapH - viewH / 2.0f;
        }

        view.setCenter(cameraPos);

        window.clear();

        window.setView(view);

        gameState.map.drawAll(window, RenderStates::Default);

        for (const auto& enemy : enemies) {
            window.draw(enemy);
        }

        window.draw(player);

        gameState.map.drawForeground(window, RenderStates::Default);

        for (const auto& effect : activeMagicEffects) {
            if (effect.currentFrame < magicTextures.size()) {
                sf::Sprite s(magicTextures[effect.currentFrame]);
                s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
                s.setPosition(effect.position);
                window.draw(s);
            }
        }

        // Overlay de morte
        if (player.checkDead()) {
            // Volta para a view padrão para desenhar na tela inteira
            window.setView(window.getDefaultView());
            sf::RectangleShape darkOverlay(sf::Vector2f(window.getSize().x, window.getSize().y));
            darkOverlay.setFillColor(sf::Color(0, 0, 0, 150)); // Preto semi-transparente
            window.draw(darkOverlay);
        }

        window.display();
    }

    return 0;
}