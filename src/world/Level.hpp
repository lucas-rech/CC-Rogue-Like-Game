#pragma once
#include <SFML/Graphics.hpp>
#include "../graphics/TileMap.hpp"
#include "../entities/hero/Player.hpp"

struct GameState {
    int currentLevel;
    TileMap map;
    bool isGameOver;
    bool isVictory;
};

void loadLevel(GameState& state, int levelIndex, Player& player);
void updateLevel(GameState& state, float deltaTime);
bool checkCollision(sf::FloatRect nextHitbox, const GameState& state);
