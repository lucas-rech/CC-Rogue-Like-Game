#pragma once
#include <SFML/Graphics.hpp>
#include "../graphics/TileMap.hpp"
#include "../entities/hero/Player.hpp"

struct GameStats {
    int score = 0;
    int movements = 0;
    int enemiesDefeated = 0;
    int itemsCollected = 0;
    int itemPoints = 0;
    int potionsUsed = 0;
    int damageTaken = 0;
    int experienceGained = 0;
    int levelsReached = 1;
    int trapsTriggered = 0;

    void updateScore(int currentLevel);
    void registerMovement(int currentLevel);
    void registerDamageTaken(int amount, int currentLevel);
    void registerItemCollected(int points, int currentLevel);
    void registerEnemyDefeated(int experienceReward, int playerLevel, int currentLevel);
    void registerPotionUsed(int currentLevel);
    void registerTrapTriggered(int currentLevel);
};

struct GameState {
    int currentLevel;
    TileMap map;
    bool isGameOver;
    bool isVictory;
    bool isGateOpen = false;
    bool playerInArena = false;
    GameStats stats;
};

void loadLevel(GameState& state, int levelIndex, Player& player);
void updateLevel(GameState& state, float deltaTime);
bool checkCollision(sf::FloatRect nextHitbox, const GameState& state);
