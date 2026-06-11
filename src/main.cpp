#include <SFML/Graphics.hpp>
#include <iostream>

#include "entities/hero/Player.hpp"
#include "world/Level.hpp"
#include "graphics/TileMap.hpp"

using namespace sf;

int main() {
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow window(VideoMode(1920, 1080), "Rogue Like Game", Style::Fullscreen, settings);
    window.setFramerateLimit(60);


    float viewWidth = 640.f;
    float viewHeight = 360.f;

    View view(FloatRect(0, 0, viewWidth, viewHeight));


    // --- 2. CONFIGURAÇÕES DO JOGADOR ---
    Player player = Player(HeroClass::Vampire, 20 * 16.0f, 10 * 16.0f);

    switch(player.getClass()) {
        case(HeroClass::Warrior):
            if (!player.loadTextures(
            "assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/idle.png",
            "assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/walk.png",
            "assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/attack.png")) {
                return -1;
            }
            break;
        case(HeroClass::Vampire):
            if (!player.loadTextures(
                "assets/textures/characters/player/vampire/vampire_lvl1/Without_shadow/idle.png",
                "assets/textures/characters/player/vampire/vampire_lvl1/Without_shadow/walk.png",
                "assets/textures/characters/player/vampire/vampire_lvl1/Without_shadow/attack.png")) {
                return -1;
            }
    }


    // --- CONFIGURAÇÕES DO MAPA ---
    GameState gameState = {0};
    loadLevel(gameState, 1, player);

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

        window.draw(player);

        gameState.map.drawForeground(window, RenderStates::Default);

        window.display();
    }

    return 0;
}