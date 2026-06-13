#include "Enemy.hpp"

#include <cmath>
#include <iostream>

Enemy::Enemy(const EnemyType type, const float startX, const float startY) {
    enemyType = type;
    
    // Variáveis gráficas
    currentFrame = 0;
    totalFrames = 12;
    textureRow = 0;
    animationSpeed = 0.08f;
    isMoving = false;
    isAttacking = false;
    isHurt = false;
    isDead = false;

    sprite.setPosition(startX, startY);

    setupStats();
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
        std::cout << "Erro ao carregar as texturas do inimigo!" << std::endl;
        return false;
    }
    sprite.setTexture(idleTexture);
    return true;
}

void Enemy::setupStats() {
    switch (enemyType) {
        case EnemyType::Orc1: //implementar
        case EnemyType::Orc2: //implementar
        case EnemyType::Orc3:
            maxHp = 50;
            speed = 0.7f;
            baseDamage = 10.0f;
            attackRange = 25.0f;
            aggroRange = 300.0f;

            frameWidth = 64;
            frameHeight = 64;
            attackFrames = 6;
            walkFrames = 6;
            idleFrames = 4;
            hurtFrames = 6;
            deathFrames = 8;

            rowDown  = 0;
            rowUp    = 1;
            rowLeft  = 2;
            rowRight = 3;
            isHurt = false;
            break;
    }
    currentHp = maxHp;
}

sf::FloatRect Enemy::getNextHitbox(sf::Vector2f playerPos, sf::Vector2f pushVector) const {
    sf::FloatRect nextHitbox = sprite.getGlobalBounds();
    nextHitbox.width = 12.f; 
    nextHitbox.height = 8.f;

    sf::Vector2f mov(0.f, 0.f);
    
    if (!isDead && !isHurt && !isAttacking) {
        float dx = playerPos.x - sprite.getPosition().x;
        float dy = playerPos.y - sprite.getPosition().y;
        float length = std::sqrt(dx * dx + dy * dy);

        if (length != 0 && length > 1.f && length <= aggroRange) {
            mov.x = (dx / length) * speed;
            mov.y = (dy / length) * speed;
        }
    }

    nextHitbox.left += mov.x + pushVector.x + 20.f;
    nextHitbox.top += mov.y + pushVector.y + 20.f;
    return nextHitbox;
}

void Enemy::updateAndMove(sf::Vector2f playerPos, bool canMove, sf::Vector2f pushVector) {
    if (isDead) {
        sprite.setTexture(deathTexture);
        totalFrames = deathFrames;
        animationSpeed = 0.1f;
        
        if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
            if (currentFrame < totalFrames - 1) {
                currentFrame++;
            }
            animationClock.restart();
        }
        int rectLeft = currentFrame * frameWidth;
        sprite.setTextureRect(sf::IntRect(rectLeft, textureRow, frameWidth, frameHeight));
        return;
    }

    if (isHurt) {
        sprite.setTexture(hurtTexture);
        totalFrames = hurtFrames;
        animationSpeed = 0.1f;

        if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
            currentFrame++;
            if (currentFrame >= totalFrames) {
                isHurt = false;
                isAttacking = false;
                currentFrame = 0;
            }
            animationClock.restart();
        }
        int rectLeft = currentFrame * frameWidth;
        sprite.setTextureRect(sf::IntRect(rectLeft, textureRow, frameWidth, frameHeight));
        return;
    }

    if (isAttacking) {
        sprite.setTexture(attackTexture);
        totalFrames = attackFrames;
        animationSpeed = 0.1f;

        if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
            currentFrame++;
            if (currentFrame >= totalFrames) {
                isAttacking = false;
                currentFrame = 0;
            }
            animationClock.restart();
        }
        int rectLeft = currentFrame * frameWidth;
        sprite.setTextureRect(sf::IntRect(rectLeft, textureRow, frameWidth, frameHeight));
        return;
    }

    // IA básica
    sf::Vector2f center = getCenterPosition();
    float dx = playerPos.x - center.x;
    float dy = playerPos.y - center.y;
    float dist = std::sqrt(dx*dx + dy*dy);
    
    sf::Vector2f movement(0.f, 0.f);
    isMoving = false;

    if (dist > 0 && dist < aggroRange && dist > attackRange) {
        movement.x = (dx / dist) * speed;
        movement.y = (dy / dist) * speed;
        isMoving = true;
    } else if (dist <= attackRange) {
        // Attack
        if (stateClock.getElapsedTime().asSeconds() > 2.5f) { // Cooldown de ataque
            attack();
            stateClock.restart();
        }
    }

    if (isMoving || pushVector.x != 0.f || pushVector.y != 0.f) {
        float angle = std::atan2(dy, dx) * 180.f / 3.14159265f;
        if (angle > -45.f && angle <= 45.f) textureRow = rowRight * frameHeight;
        else if (angle > 45.f && angle <= 135.f) textureRow = rowDown * frameHeight;
        else if (angle > 135.f || angle <= -135.f) textureRow = rowLeft * frameHeight;
        else if (angle > -135.f && angle <= -45.f) textureRow = rowUp * frameHeight;
        
        if (canMove) {
            sprite.move(movement + pushVector);
        }
    }

    if (isMoving) {
        sprite.setTexture(walkTexture);
        totalFrames = walkFrames;
        animationSpeed = 0.1f;
    } else {
        sprite.setTexture(idleTexture);
        totalFrames = idleFrames;
        animationSpeed = 0.12f;
    }

    if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
        currentFrame++;
        if (currentFrame >= totalFrames) {
            currentFrame = 0;
        }
        animationClock.restart();
    }

    int rectLeft = currentFrame * frameWidth;
    sprite.setTextureRect(sf::IntRect(rectLeft, textureRow, frameWidth, frameHeight));
}

void Enemy::attack() {
    isAttacking = true;
    currentFrame = 0;
    justAttacked = true;
}

void Enemy::takeDamage(int amount) {
    if (isDead) return;
    
    currentHp -= amount;
    if (currentHp <= 0) {
        currentHp = 0;
        isDead = true;
        currentFrame = 0;
    } else {
        isHurt = true;
        currentFrame = 0;
    }
}

sf::FloatRect Enemy::getNextHitbox(sf::Vector2f movement) const {
    sf::FloatRect nextHitbox = sprite.getGlobalBounds();
    // Ajuste da hitbox similar ao Player
    nextHitbox.width = 12.f; 
    nextHitbox.height = 8.f;
    nextHitbox.left += movement.x + 26.f; 
    nextHitbox.top += movement.y + 20.f;
    return nextHitbox;
}

sf::FloatRect Enemy::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

sf::Vector2f Enemy::getPosition() const {
    return sprite.getPosition();
}

sf::Vector2f Enemy::getCenterPosition() const {
    return {sprite.getPosition().x + 32.f, sprite.getPosition().y + 32.f};
}

EnemyType Enemy::getType() const {
    return enemyType;
}

void Enemy::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void Enemy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(sprite, states);
}
