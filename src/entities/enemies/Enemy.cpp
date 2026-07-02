#include "Enemy.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <random>
#include <iostream>

#include "../../items/Item.hpp"
#include "../../entities/hero/Player.hpp"
#include "../../world/Level.hpp"

namespace {
    std::mt19937& randomGenerator() {
        static std::mt19937 generator(std::random_device{}());
        return generator;
    }

    float distanceBetween(sf::Vector2f a, sf::Vector2f b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    bool isValidSpawn(sf::Vector2f position, const GameState& gameState, const Player& player) {
        sf::FloatRect spawnArea(position.x + 22.f, position.y + 28.f, 20.f, 18.f);
        return !checkCollision(spawnArea, gameState)
            && distanceBetween(position, player.getPosition()) > 120.f;
    }
}

Enemy::Enemy(EnemyType type, sf::Vector2f startPosition, Difficulty difficulty)
    : type(type),
      difficulty(difficulty),
      currentHp(1),
      maxHp(1),
      damage(1),
      expReward(10),
      speed(20.f),
      detectionRange(120.f),
      attackRange(24.f),
      attackCooldown(1.f),
      attackTimer(0.f),
      frameWidth(64),
      frameHeight(64),
      idleFrames(4),
      walkFrames(6),
      attackFrames(6),
      hurtFrames(6),
      deathFrames(8),
      rowDown(0),
      rowUp(1),
      rowLeft(2),
      rowRight(3),
      currentFrame(0),
      totalFrames(4),
      textureRow(0),
      animationSpeed(0.1f),
      alive(true),
      isMoving(false),
      isAttacking(false),
      isHurt(false),
      isDeadAnimationComplete(false),
      attackDamagePending(false),
      randomDirection(0.f, 0.f),
      randomMoveTimer(0.f),
      randomMoveDuration(1.5f) {
    setupStats();
    sprite.setPosition(startPosition);
    chooseRandomDirection();
}

bool Enemy::loadTexture() {
    std::string basePath;

    switch (type) {
        case EnemyType::Goblin:
            basePath = "assets/textures/characters/enemies/Orc1/Without_shadow/";
            break;
        case EnemyType::Orc:
            basePath = "assets/textures/characters/enemies/Orc2/Without_shadow/";
            break;
        case EnemyType::Brute:
            basePath = "assets/textures/characters/enemies/Orc3/Without_shadow/";
            break;
        case EnemyType::Boss:
            basePath = "assets/textures/characters/player/vampire/vampire_lvl1/Without_shadow/";
            break;
    }

    if (loadTextures(basePath + "idle.png",
                     basePath + "walk.png",
                     basePath + "attack.png",
                     basePath + "hurt.png",
                     basePath + "death.png")) {
        return true;
    }

    sf::Image image;
    image.create(32, 32, sf::Color::Red);
    idleTexture.loadFromImage(image);
    sprite.setTexture(idleTexture);
    return false;
}

bool Enemy::loadTextures(const std::string& idlePath,
                         const std::string& walkPath,
                         const std::string& attackPath,
                         const std::string& hurtPath,
                         const std::string& deathPath) {
    if (!idleTexture.loadFromFile(idlePath) ||
        !walkTexture.loadFromFile(walkPath) ||
        !attackTexture.loadFromFile(attackPath) ||
        !hurtTexture.loadFromFile(hurtPath) ||
        !deathTexture.loadFromFile(deathPath)) {
        return false;
    }

    sprite.setTexture(idleTexture);
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    return true;
}

void Enemy::update(float deltaTime, Player& player, GameState& gameState) {
    attackTimer += deltaTime;
    itemSearchTimer += deltaTime;

    if (!alive) {
        updateAnimation();
        return;
    }

    if (isHurt || isAttacking) {
        updateAnimation();
    } else {
        if (itemSearchTimer > 2.0f) {
            itemSearchTimer = 0.f;
            // 20% de chance de tentar pegar um item
            if (static_cast<int>(randomGenerator()() % 100) < 20) {
                chasingItem = true;
            } else {
                chasingItem = false;
            }
        }

        if (chasingItem && targetItemPos != sf::Vector2f(0, 0) && chaseNearestItem(deltaTime, gameState, targetItemPos)) {
            updateAnimation();
        } else if (canSeePlayer(player)) {
            chasingItem = false;
            chasePlayer(deltaTime, player, gameState);
            updateAnimation();
        } else {
            chasingItem = false;
            moveRandom(deltaTime, gameState);
            updateAnimation();
        }
    }

    if (canAttackPlayer(player) && attackTimer >= attackCooldown && !isAttacking && !isHurt) {
        startAttack();
        attackTimer = 0.f;
    }

    if (attackDamagePending && canAttackPlayer(player)) {
        int actualDamage = player.takeDamage(damage);
        gameState.stats.registerDamageTaken(actualDamage, gameState.currentLevel);
        attackDamagePending = false;

        if (player.getCurrentHp() <= 0) {
            gameState.isGameOver = true;
        }
    }
}

void Enemy::takeDamage(int amount) {
    if (!alive) {
        return;
    }

    currentHp -= amount;
    currentFrame = 0;

    if (currentHp <= 0) {
        currentHp = 0;
        alive = false;
        isHurt = false;
        isAttacking = false;
        isDeadAnimationComplete = false;
    } else {
        isHurt = true;
        isAttacking = false;
    }
}

bool Enemy::isAlive() const {
    return alive;
}

bool Enemy::canAttackPlayer(const Player& player) const {
    return alive && distanceBetween(getCenterPosition(), player.getCenterPosition()) <= attackRange;
}

int Enemy::getExpReward() const {
    return expReward;
}

EnemyType Enemy::getType() const {
    return type;
}

void Enemy::setTargetItemPos(sf::Vector2f pos) {
    targetItemPos = pos;
}

bool Enemy::isDeadComplete() const {
    return isDeadAnimationComplete;
}

sf::FloatRect Enemy::getHitbox() const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    bounds.left += 22.f;
    bounds.top += 28.f;
    bounds.width = 20.f;
    bounds.height = 18.f;
    return bounds;
}

void Enemy::setupStats() {
    const DifficultyConfig& config = getDifficultyConfig(difficulty);

    switch (type) {
        case EnemyType::Goblin:
            maxHp = config.goblin.maxHp;
            damage = config.goblin.damage;
            expReward = 20;
            speed = 30.f;
            detectionRange = 180.f;
            attackRange = 28.f;
            attackCooldown = 1.0f;
            break;
        case EnemyType::Orc:
            maxHp = config.orc.maxHp;
            damage = config.orc.damage;
            expReward = 35;
            speed = 24.f;
            detectionRange = 220.f;
            attackRange = 30.f;
            attackCooldown = 1.2f;
            break;
        case EnemyType::Brute:
            maxHp = config.brute.maxHp;
            damage = config.brute.damage;
            expReward = 55;
            speed = 18.f;
            detectionRange = 260.f;
            attackRange = 34.f;
            attackCooldown = 1.6f;
            break;
        case EnemyType::Boss:
            maxHp = config.boss.maxHp;
            damage = config.boss.damage;
            expReward = 1000;
            speed = 40.f;
            detectionRange = 1500.f; // huge range to chase player
            attackRange = 30.f;
            attackCooldown = 0.8f;
            idleFrames = 4;
            walkFrames = 6;
            attackFrames = 12;
            hurtFrames = 4;
            deathFrames = 10;
            break;
    }

    currentHp = maxHp;
}

void Enemy::moveRandom(float deltaTime, const GameState& gameState) {
    randomMoveTimer += deltaTime;

    if (randomMoveTimer >= randomMoveDuration) {
        chooseRandomDirection();
    }

    sf::Vector2f movement = randomDirection * speed * deltaTime;
    sf::Vector2f oldPosition = sprite.getPosition();
    isMoving = movement.x != 0.f || movement.y != 0.f;
    setFacingFromVector(movement);
    tryMove(movement, gameState);

    if (sprite.getPosition() == oldPosition && isMoving) {
        chooseRandomDirection();
    }
}

void Enemy::chasePlayer(float deltaTime, const Player& player, const GameState& gameState) {
    sf::Vector2f toPlayer = player.getCenterPosition() - getCenterPosition();
    float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

    if (distance <= attackRange || distance == 0.f) {
        isMoving = false;
        setFacingFromVector(toPlayer);
        return;
    }

    sf::Vector2f movement(toPlayer.x / distance * speed * deltaTime,
                          toPlayer.y / distance * speed * deltaTime);

    isMoving = true;
    setFacingFromVector(movement);
    tryMove(movement, gameState);
}

bool Enemy::chaseNearestItem(float deltaTime, const GameState& gameState, const sf::Vector2f& itemPos) {
    sf::Vector2f toItem = itemPos - getCenterPosition();
    float distance = std::sqrt(toItem.x * toItem.x + toItem.y * toItem.y);

    if (distance > 30.f) {
        sf::Vector2f movement(toItem.x / distance * speed * deltaTime,
                              toItem.y / distance * speed * deltaTime);
        isMoving = true;
        setFacingFromVector(movement);
        tryMove(movement, gameState);
    } else {
        isMoving = false;
    }
    return true;
}

bool Enemy::canSeePlayer(const Player& player) const {
    return distanceBetween(getCenterPosition(), player.getCenterPosition()) <= detectionRange;
}

void Enemy::tryMove(sf::Vector2f movement, const GameState& gameState) {
    if (movement.x != 0.f) {
        sprite.move(movement.x, 0.f);
        if (collidesWithMap(getHitbox(), gameState)) {
            sprite.move(-movement.x, 0.f);
        }
    }

    if (movement.y != 0.f) {
        sprite.move(0.f, movement.y);
        if (collidesWithMap(getHitbox(), gameState)) {
            sprite.move(0.f, -movement.y);
        }
    }
}

bool Enemy::collidesWithMap(sf::FloatRect hitbox, const GameState& gameState) const {
    return checkCollision(hitbox, gameState);
}

void Enemy::chooseRandomDirection() {
    std::uniform_int_distribution<int> directionDistribution(0, 4);
    std::uniform_real_distribution<float> timeDistribution(1.0f, 2.0f);

    switch (directionDistribution(randomGenerator())) {
        case 0: randomDirection = {1.f, 0.f}; break;
        case 1: randomDirection = {-1.f, 0.f}; break;
        case 2: randomDirection = {0.f, 1.f}; break;
        case 3: randomDirection = {0.f, -1.f}; break;
        default: randomDirection = {0.f, 0.f}; break;
    }

    randomMoveDuration = timeDistribution(randomGenerator());
    randomMoveTimer = 0.f;
}

void Enemy::setFacingFromVector(sf::Vector2f direction) {
    if (direction.x == 0.f && direction.y == 0.f) {
        return;
    }

    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159265f;
    if (angle > -45.f && angle <= 45.f) textureRow = rowRight * frameHeight;
    else if (angle > 45.f && angle <= 135.f) textureRow = rowDown * frameHeight;
    else if (angle > 135.f || angle <= -135.f) textureRow = rowLeft * frameHeight;
    else if (angle > -135.f && angle <= -45.f) textureRow = rowUp * frameHeight;
}

void Enemy::updateAnimation() {
    if (!alive) {
        sprite.setTexture(deathTexture);
        totalFrames = deathFrames;
        animationSpeed = 0.1f;
    } else if (isHurt) {
        sprite.setTexture(hurtTexture);
        totalFrames = hurtFrames;
        animationSpeed = 0.1f;
    } else if (isAttacking) {
        sprite.setTexture(attackTexture);
        totalFrames = attackFrames;
        animationSpeed = 0.1f;
    } else if (isMoving) {
        sprite.setTexture(walkTexture);
        totalFrames = walkFrames;
        animationSpeed = 0.1f;
    } else {
        sprite.setTexture(idleTexture);
        totalFrames = idleFrames;
        animationSpeed = 0.12f;
    }

    if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
        if (!alive) {
            if (currentFrame < totalFrames - 1) {
                currentFrame++;
            } else {
                isDeadAnimationComplete = true;
            }
        } else {
            currentFrame++;
            if (currentFrame >= totalFrames) {
                currentFrame = 0;
                isHurt = false;
                isAttacking = false;
            }
        }

        animationClock.restart();
    }

    sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, textureRow, frameWidth, frameHeight));
}

void Enemy::startAttack() {
    isAttacking = true;
    attackDamagePending = true;
    currentFrame = 0;
}

sf::Vector2f Enemy::getCenterPosition() const {
    return {sprite.getPosition().x + 32.f, sprite.getPosition().y + 32.f};
}

void Enemy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (alive || !isDeadAnimationComplete) {
        target.draw(sprite, states);
    }
}

void spawnEnemies(std::vector<Enemy>& enemies, const GameState& gameState, const Player& player) {
    enemies.clear();
    const DifficultyConfig& config = getDifficultyConfig(gameState.difficulty);
    enemies.reserve(21 + config.randomEnemyCount);

    const std::array<std::array<sf::Vector2f, 2>, 3> hordeCenters{{
        std::array<sf::Vector2f, 2>{sf::Vector2f{527.15f, 1462.45f}, sf::Vector2f{743.3f, 968.35f}},
        std::array<sf::Vector2f, 2>{sf::Vector2f{1120.f, 1540.f}, sf::Vector2f{1840.f, 980.f}},
        std::array<sf::Vector2f, 2>{sf::Vector2f{620.f, 520.f}, sf::Vector2f{2200.f, 1120.f}}
    }};
    const int levelIndex = std::max(1, std::min(gameState.currentLevel, gameState.maxLevel)) - 1;

    auto spawnHorde = [&](sf::Vector2f center, int count) {
        for (int i = 0; i < count; ++i) {
            float offsetX = static_cast<float>(static_cast<int>(randomGenerator()() % 100) - 50);
            float offsetY = static_cast<float>(static_cast<int>(randomGenerator()() % 100) - 50);
            sf::Vector2f pos(center.x + offsetX, center.y + offsetY);
            
            EnemyType type = randomGenerator()() % 2 == 0 ? EnemyType::Orc : EnemyType::Brute;
            enemies.emplace_back(type, pos, gameState.difficulty);
            enemies.back().loadTexture();
        }
    };

    spawnHorde(hordeCenters[levelIndex][0], 10);
    spawnHorde(hordeCenters[levelIndex][1], 10);

    if (gameState.currentLevel == gameState.maxLevel) {
        enemies.emplace_back(EnemyType::Boss, sf::Vector2f(2712.05f, 165.25f), gameState.difficulty);
        enemies.back().loadTexture();
    }

    std::uniform_int_distribution<int> xDist(2, gameState.map.getWidth() - 3);
    std::uniform_int_distribution<int> yDist(2, gameState.map.getHeight() - 3);

    for (int i = 0; i < config.randomEnemyCount; ++i) {
        bool spawned = false;
        int attempts = 0;
        while (!spawned && attempts < 100) {
            int x = xDist(randomGenerator());
            int y = yDist(randomGenerator());
            sf::Vector2f position(static_cast<float>(x * gameState.map.getTileSize()),
                                  static_cast<float>(y * gameState.map.getTileSize()));

            if (isValidSpawn(position, gameState, player)) {
                EnemyType type = enemies.size() % 3 == 0 ? EnemyType::Goblin
                               : enemies.size() % 3 == 1 ? EnemyType::Orc
                                                         : EnemyType::Brute;
                enemies.emplace_back(type, position, gameState.difficulty);
                enemies.back().loadTexture();
                spawned = true;
            }
            attempts++;
        }
    }
}

void spawnRandomEnemy(std::vector<Enemy>& enemies, const GameState& gameState, const Player& player) {
    if (gameState.currentLevel == gameState.maxLevel && gameState.playerInArena) return; // Don't spawn in boss arena

    std::uniform_int_distribution<int> xDist(2, gameState.map.getWidth() - 3);
    std::uniform_int_distribution<int> yDist(2, gameState.map.getHeight() - 3);

    std::mt19937 randGen(std::random_device{}());

    bool spawned = false;
    int attempts = 0;
    while (!spawned && attempts < 50) {
        int x = xDist(randGen);
        int y = yDist(randGen);
        sf::Vector2f pos(static_cast<float>(x * gameState.map.getTileSize()),
                         static_cast<float>(y * gameState.map.getTileSize()));

        bool farFromPlayer = (std::abs(pos.x - player.getCenterPosition().x) > 300.f ||
                              std::abs(pos.y - player.getCenterPosition().y) > 300.f);

        bool validPos = isValidSpawn(pos, gameState, player) && farFromPlayer;
        if (validPos) {
            Enemy enemy(EnemyType::Goblin, pos, gameState.difficulty);
            if (enemy.loadTexture()) {
                enemies.push_back(enemy);
                spawned = true;
                std::cout << "[FARM] Inimigo randomico gerado em " << pos.x << ", " << pos.y << "\n";
            }
        }
        attempts++;
    }
}

void updateEnemies(std::vector<Enemy>& enemies, Player& player, GameState& gameState, float deltaTime) {
    for (Enemy& enemy : enemies) {
        enemy.update(deltaTime, player, gameState);
    }
}

void drawEnemies(sf::RenderWindow& window, const std::vector<Enemy>& enemies) {
    for (const Enemy& enemy : enemies) {
        window.draw(enemy);
    }
}

void handlePlayerAttack(std::vector<Enemy>& enemies, Player& player, GameState& gameState) {
    if (!player.consumeAttackDamageReady()) {
        return;
    }

    sf::Vector2f playerCenter = player.getCenterPosition();
    sf::Vector2f targetPos = player.getTargetPos();
    sf::Vector2f aimDir = player.getAimDirection();
    float attackRange = player.getAttackRange();

    for (Enemy& enemy : enemies) {
        if (!enemy.isAlive()) {
            continue;
        }

        bool hit = false;
        sf::Vector2f enemyCenter = enemy.getCenterPosition();
        float dx = enemyCenter.x - playerCenter.x;
        float dy = enemyCenter.y - playerCenter.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (!player.isPendingAttackMagic()) {
            if (distance <= attackRange && distance > 0.f) {
                sf::Vector2f dirToEnemy(dx / distance, dy / distance);
                float dotProduct = dirToEnemy.x * aimDir.x + dirToEnemy.y * aimDir.y;
                hit = dotProduct > 0.35f;
            }
        } else {
            float tx = enemyCenter.x - targetPos.x;
            float ty = enemyCenter.y - targetPos.y;
            hit = distanceBetween(enemyCenter, targetPos) <= 60.f
               && distanceBetween(playerCenter, targetPos) <= 200.f; // Magic range was 200
        }

        if (!hit) {
            continue;
        }

        enemy.takeDamage(player.getDamage());

        if (!enemy.isAlive()) {
            player.gainExp(enemy.getExpReward());
            gameState.stats.registerEnemyDefeated(enemy.getExpReward(), player.getLevel(), gameState.currentLevel);
            gameState.campaignScore += 100;
            if (enemy.getType() == EnemyType::Boss && gameState.currentLevel == gameState.maxLevel) {
                gameState.isVictory = true;
            }
        }
    }
}
