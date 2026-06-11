#pragma once
#include <SFML/Graphics.hpp>
#include "../graphics/TileMap.hpp"

struct GameState {
    int currentLevel;
    TileMap map;
    bool isGameOver;
    bool isVictory;
};

void loadLevel(GameState& state, int levelIndex, sf::Sprite& playerSprite);
void updateLevel(GameState& state, float deltaTime);
bool checkCollision(sf::FloatRect nextHitbox, const GameState& state);
