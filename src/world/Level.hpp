#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "../graphics/TileMap.hpp"
#include "../entities/hero/Player.hpp"

enum class Difficulty {
    Easy,
    Medium,
    Hard
};

struct EnemyStats {
    int maxHp;
    int damage;
};

struct DifficultyConfig {
    int randomEnemyCount;
    int healthPotionCount;
    int manaPotionCount;
    int shieldCount;
    EnemyStats goblin;
    EnemyStats orc;
    EnemyStats brute;
    EnemyStats boss;
};

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
    std::string playerName = "Jogador";
    int maxLevel = 1;
    Difficulty difficulty = Difficulty::Easy;
    TileMap map;
    bool isGameOver;
    bool isVictory;
    bool isGateOpen = false;
    bool playerInArena = false;
    int campaignScore = 0;
    int completedLevels = 0;
    int finalScore = 0;
    float campaignElapsedTime = 0.f;
    bool campaignActive = false;
    bool finalScoreCalculated = false;
    bool isAutoPlay = false;
    GameStats stats;
};

const DifficultyConfig& getDifficultyConfig(Difficulty difficulty);
std::string getDifficultyName(Difficulty difficulty);
std::string formatCampaignTime(float elapsedTime);
int calculateFinalCampaignScore(const GameState& state, int currentHp, bool victory);
void loadLevel(GameState& state, int levelIndex, Player& player);
void updateLevel(GameState& state, float deltaTime);
bool checkCollision(sf::FloatRect nextHitbox, const GameState& state);
