//
// Created by lucas on 11/06/2026.
//
#include "Level.hpp"

class Player;
using namespace sf;

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
    state.currentLevel = levelIndex;
    
    if (levelIndex == 1) {
        if (!state.map.loadFromJson("world/mapa_lvl_1.tmj")) {
            if (!state.map.loadFromJson("src/world/mapa_lvl_1.tmj")) {
                state.map.loadFromJson("../src/world/mapa_lvl_1.tmj");
            }
        }

        // Reposiciona o jogador numa área livre
        player.setPosition(20 * 16.0f, 10 * 16.0f);
    }
    

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
