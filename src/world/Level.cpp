//
// Created by lucas on 11/06/2026.
//
#include "Level.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

class Player;
using namespace sf;

namespace {
    const DifficultyConfig easyConfig{
        25, 15, 15, 5,
        {35, 5}, {60, 8}, {100, 14}, {200, 40}
    };

    const DifficultyConfig mediumConfig{
        34, 12, 12, 4,
        {42, 6}, {72, 10}, {120, 17}, {350, 48}
    };

    const DifficultyConfig hardConfig{
        43, 9, 9, 3,
        {49, 7}, {84, 11}, {140, 20}, {500, 56}
    };
}

const DifficultyConfig& getDifficultyConfig(Difficulty difficulty) {
    switch (difficulty) {
        case Difficulty::Medium:
            return mediumConfig;
        case Difficulty::Hard:
            return hardConfig;
        case Difficulty::Easy:
        default:
            return easyConfig;
    }
}

std::string getDifficultyName(Difficulty difficulty) {
    switch (difficulty) {
        case Difficulty::Medium:
            return "Medio";
        case Difficulty::Hard:
            return "Dificil";
        case Difficulty::Easy:
        default:
            return "Facil";
    }
}
std::string formatCampaignTime(float elapsedTime) {
    int totalSeconds = static_cast<int>(elapsedTime);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    std::ostringstream output;
    output << std::setfill('0') << std::setw(2) << minutes
           << ":" << std::setw(2) << seconds;
    return output.str();
}

int calculateFinalCampaignScore(const GameState& state, int currentHp, bool victory) {
    int totalSeconds = static_cast<int>(state.campaignElapsedTime);
    int score = state.campaignScore
              + (std::max(0, currentHp) * 10)
              + (victory ? 1000 : 0)
              - totalSeconds;

    return std::max(0, score);
}

void GameStats::updateScore(int currentLevel) {
    score = (enemiesDefeated * 100)
          + itemPoints
          + (experienceGained * 1)
          + (levelsReached * 100)
          + (currentLevel * 200)
          - (movements * 1)
          - (damageTaken * 2)
          - (potionsUsed * 10)
          - (trapsTriggered * 25);

    if (score < 0) {
        score = 0;
    }
}

void GameStats::registerMovement(int currentLevel) {
    movements++;
    updateScore(currentLevel);
}

void GameStats::registerDamageTaken(int amount, int currentLevel) {
    damageTaken += amount;
    updateScore(currentLevel);
}

void GameStats::registerItemCollected(int points, int currentLevel) {
    itemsCollected++;
    itemPoints += points;
    updateScore(currentLevel);
}

void GameStats::registerEnemyDefeated(int experienceReward, int playerLevel, int currentLevel) {
    enemiesDefeated++;
    experienceGained += experienceReward;
    levelsReached = playerLevel;
    updateScore(currentLevel);
}

void GameStats::registerPotionUsed(int currentLevel) {
    potionsUsed++;
    updateScore(currentLevel);
}

void GameStats::registerTrapTriggered(int currentLevel) {
    trapsTriggered++;
    updateScore(currentLevel);
}

void loadLevel(GameState& state, int levelIndex, Player& player) {
    if (levelIndex < 1) {
        levelIndex = 1;
    } else if (levelIndex > state.maxLevel) {
        levelIndex = state.maxLevel;
    }

    state.currentLevel = levelIndex;

    const std::string mapFile = "mapa_lvl_" + std::to_string(levelIndex) + ".tmj";
    if (!state.map.loadFromJson("world/" + mapFile)) {
        if (!state.map.loadFromJson("src/world/" + mapFile)) {
            state.map.loadFromJson("../src/world/" + mapFile);
        }
    }

    state.isGateOpen = false;
    state.playerInArena = false;
    player.setPosition(20 * 16.0f, 10 * 16.0f);
}

bool checkCollision(FloatRect nextHitbox, const GameState& state) {
    if (state.isGateOpen && !state.playerInArena) {
        // Ignore collision in the gate's area to pass through
        if (nextHitbox.top < 660.f && nextHitbox.top + nextHitbox.height > 640.f && 
            nextHitbox.left + nextHitbox.width > 2400.f && nextHitbox.left < 2800.f) {
            return false;
        }
    }

    if (state.playerInArena) {
        // Cannot go back down (wall from 640 to 660)
        if (nextHitbox.top < 660.f && nextHitbox.top + nextHitbox.height > 640.f && 
            nextHitbox.left + nextHitbox.width > 2400.f && nextHitbox.left < 2800.f) {
            return true;
        }
    } else if (!state.isGateOpen) {
        // Barricade wall from 640 to 660 when closed
        if (nextHitbox.top < 660.f && nextHitbox.top + nextHitbox.height > 640.f && 
            nextHitbox.left + nextHitbox.width > 2400.f && nextHitbox.left < 2800.f) {
            return true;
        }
    }

    if (state.map.checkCollision(nextHitbox.left, nextHitbox.top)) return true;
    if (state.map.checkCollision(nextHitbox.left + nextHitbox.width, nextHitbox.top)) return true;
    if (state.map.checkCollision(nextHitbox.left, nextHitbox.top + nextHitbox.height)) return true;
    if (state.map.checkCollision(nextHitbox.left + nextHitbox.width, nextHitbox.top + nextHitbox.height)) return true;

    return false;
}
