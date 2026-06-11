//
// Created by lucas on 11/06/2026.
//
#include "Level.hpp"
#include "SFML/Graphics/Sprite.hpp"

using namespace sf;

#include "Level.hpp"
#include "SFML/Graphics/Sprite.hpp"

using namespace sf;

void loadLevel(GameState& state, int levelIndex, sf::Sprite& playerSprite) {
    state.currentLevel = levelIndex;
    
    if (levelIndex == 1) {
        // Tenta os três caminhos comuns dependendo do Working Directory do CLion
        if (!state.map.loadFromJson("../src/world/mapa_fase1.tmj")) {
            if (!state.map.loadFromJson("src/world/mapa_fase1.tmj")) {
                state.map.loadFromJson("world/mapa_fase1.tmj");
            }
        }
    }
    
    // Reposiciona o jogador numa área livre
    playerSprite.setPosition(6 * 16.0f, 8 * 16.0f);
}

bool checkCollision(FloatRect nextHitbox, const GameState& state) {
    // Verifica os 4 cantos da hitbox nos blocos de colisão do TileMap
    if (state.map.checkCollision(nextHitbox.left, nextHitbox.top)) return true;
    if (state.map.checkCollision(nextHitbox.left + nextHitbox.width, nextHitbox.top)) return true;
    if (state.map.checkCollision(nextHitbox.left, nextHitbox.top + nextHitbox.height)) return true;
    if (state.map.checkCollision(nextHitbox.left + nextHitbox.width, nextHitbox.top + nextHitbox.height)) return true;

    return false;
}