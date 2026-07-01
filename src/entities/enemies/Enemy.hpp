#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "../../world/Level.hpp"
#include <vector>

class Player;
class Item;

enum class EnemyType {
    Goblin,
    Orc,
    Brute,
    Boss
};

class Enemy : public sf::Drawable {
public:
    Enemy(EnemyType type, sf::Vector2f startPosition, Difficulty difficulty = Difficulty::Easy);

    bool loadTexture();
    void update(float deltaTime, Player& player, GameState& gameState);
    void takeDamage(int amount);
    bool isAlive() const;
    bool canAttackPlayer(const Player& player) const;
    int getExpReward() const;
    EnemyType getType() const;
    void setTargetItemPos(sf::Vector2f pos);
    bool isDeadComplete() const;
    sf::FloatRect getHitbox() const;
    sf::Vector2f getCenterPosition() const;

private:
    sf::Sprite sprite;
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Texture attackTexture;
    sf::Texture hurtTexture;
    sf::Texture deathTexture;
    EnemyType type;
    Difficulty difficulty;

    int currentHp;
    int maxHp;
    int damage;
    int expReward;
    float speed;
    float detectionRange;
    float attackRange;
    float attackCooldown;
    float attackTimer;

    int frameWidth;
    int frameHeight;
    int idleFrames;
    int walkFrames;
    int attackFrames;
    int hurtFrames;
    int deathFrames;
    int rowDown;
    int rowUp;
    int rowLeft;
    int rowRight;
    int currentFrame;
    int totalFrames;
    int textureRow;
    float animationSpeed;

    bool alive;
    bool isMoving;
    bool isAttacking;
    bool isHurt;
    bool isDeadAnimationComplete;
    bool attackDamagePending;

    sf::Vector2f randomDirection;
    float randomMoveTimer;
    float randomMoveDuration;
    sf::Clock animationClock;

    float itemSearchTimer = 0.f;
    bool chasingItem = false;
    sf::Vector2f targetItemPos;

    void setupStats();
    bool loadTextures(const std::string& idlePath,
                      const std::string& walkPath,
                      const std::string& attackPath,
                      const std::string& hurtPath,
                      const std::string& deathPath);
    void moveRandom(float deltaTime, const GameState& gameState);
    void chasePlayer(float deltaTime, const Player& player, const GameState& gameState);
    bool chaseNearestItem(float deltaTime, const GameState& gameState, const sf::Vector2f& itemPos);
    bool canSeePlayer(const Player& player) const;
    void tryMove(sf::Vector2f movement, const GameState& gameState);
    bool collidesWithMap(sf::FloatRect hitbox, const GameState& gameState) const;
    void chooseRandomDirection();
    void setFacingFromVector(sf::Vector2f direction);
    void updateAnimation();
    void startAttack();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

void spawnEnemies(std::vector<Enemy>& enemies, const GameState& gameState, const Player& player);
void spawnRandomEnemy(std::vector<Enemy>& enemies, const GameState& gameState, const Player& player);
void updateEnemies(std::vector<Enemy>& enemies, Player& player, GameState& gameState, float deltaTime);
void drawEnemies(sf::RenderWindow& window, const std::vector<Enemy>& enemies);
void handlePlayerAttack(std::vector<Enemy>& enemies, Player& player, GameState& gameState);
