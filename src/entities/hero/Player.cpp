#include "Player.hpp"

#include <cmath>
#include <iostream>

Player::Player(const HeroClass type, const float startX, const float startY) {
    heroClass = type;
    level = 1;
    currentExp = 0;
    maxExp = 100;
    potions = 3;
    currentHp = maxHp;

    //variáveis gráficas
    playerFrame = 0;
    totalFrames = 12;
    textureRow = 0;
    animationSpeed = 0.08f;
    isMoving = false;
    isAttacking = false;
    sprite.setPosition(startX, startY);

    setupClassStats();
}

bool Player::loadTextures(const std::string& idlePath, const std::string& walkPath, const std::string& attackPath) {
    if (!idleTexture.loadFromFile(idlePath) ||
        !walkTexture.loadFromFile(walkPath) ||
        !attackTexture.loadFromFile(attackPath)) {
        std::cout << "Erro ao carregar as texturas do jogador!" << std::endl;
        return false;
    }
    sprite.setTexture(idleTexture);
    return true;
}

void Player::setupClassStats() {
    switch (heroClass) {
        case HeroClass::Warrior:
            maxHp = 100;
            speed = 1.5f;
            baseDamage = 15.0f;

            frameWidth = 64;
            frameHeight = 64;
            attackFrames = 8;
            walkFrames = 6;
            idleFrames = 12;

            //linhas do sprite sheet
            rowDown  = 0;
            rowLeft  = 1;
            rowRight = 2;
            rowUp    = 3;
            break;

        case HeroClass::Vampire:
            maxHp = 60;
            speed = 1.2f;
            baseDamage = 25.0f;

            frameWidth = 64;
            frameHeight = 64;
            attackFrames = 12;
            walkFrames = 6;
            idleFrames = 4;

            rowDown  = 0;
            rowUp    = 1;
            rowLeft  = 2;
            rowRight = 3;
            break;

        case HeroClass::Archer:
            maxHp = 80;
            speed = 1.8f;
            baseDamage = 10.0f;
            break;
    }
    currentHp = maxHp;
}

void Player::processInput(const sf::RenderWindow& window) {
    currentMovement = sf::Vector2f(0.f, 0.f);
    isMoving = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { currentMovement.x += speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { currentMovement.x -= speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { currentMovement.y += speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { currentMovement.y -= speed; isMoving = true; }

    const sf::Vector2i currentMousePixelPosition = sf::Mouse::getPosition(window);
    const sf::Vector2f mouseWorldPos = window.mapPixelToCoords(currentMousePixelPosition);
    const sf::Vector2f playerCenter = getCenterPosition();

    float dx = mouseWorldPos.x - playerCenter.x;
    float dy = mouseWorldPos.y - playerCenter.y;
    aimAngle = std::atan2(dy, dx) * 180.f / 3.14159265f;

    if (aimAngle > -45.f && aimAngle <= 45.f) {
        textureRow = rowRight * frameHeight; // Direita
    }
    else if (aimAngle > 45.f && aimAngle <= 135.f) {
        textureRow = rowDown * frameHeight; // Baixo
    }
    else if (aimAngle > 135.f || aimAngle <= -135.f) {
        textureRow = rowLeft * frameHeight; // Esquerda
    }
    else if (aimAngle > -135.f && aimAngle <= -45.f) {
        textureRow = rowUp * frameHeight; // Cima
    }

    float length = std::sqrt(dx * dx + dy * dy);
    if (length != 0) {
        aimDirection = sf::Vector2f(dx / length, dy / length);
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        attack();
    }
}

sf::FloatRect Player::getNextHitbox() const {
    sf::FloatRect nextHitbox = sprite.getGlobalBounds();
    nextHitbox.width = 12.f; 
    nextHitbox.height = 8.f;
    nextHitbox.left += currentMovement.x + 26.f; 
    nextHitbox.top += currentMovement.y + 20.f;
    return nextHitbox;
}

void Player::updateAndMove(bool canMove) {
    if (isMoving && canMove) {
        sf::Vector2f actualMovement = currentMovement;

        if (isAttacking) {
            actualMovement.x *= 0.1f;
            actualMovement.y *= 0.1f;
        }

        sprite.move(actualMovement);
    }

    if (isAttacking) {
        sprite.setTexture(attackTexture);
        totalFrames = attackFrames;
        animationSpeed = 0.05f;

    } else if (isMoving) {
        sprite.setTexture(walkTexture);
        totalFrames = walkFrames;
        animationSpeed = 0.1f;
    } else {
        sprite.setTexture(idleTexture);
        animationSpeed = 0.12f;
        if (heroClass == HeroClass::Warrior && textureRow == rowUp * frameHeight) {
            totalFrames = 4;
        } else {
            totalFrames = idleFrames;
        }
    }

    // --- CONTROLE DA ANIMAÇÃO DO SPRITE ---
    if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
        playerFrame++;

        if (playerFrame >= totalFrames) {
            playerFrame = 0;

            if (isAttacking) {
                isAttacking = false;
            }
        }

        animationClock.restart();
    }
    //if (playerFrame >= totalFrames) playerFrame = 0;

    int rectLeft = playerFrame * frameWidth;
    sprite.setTextureRect(sf::IntRect(rectLeft, textureRow, frameWidth, frameHeight));
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(sprite, states);
}

void Player::takeDamage(int amount) {
    currentHp -= amount;
    if (currentHp < 0) currentHp = 0;
}

void Player::heal(int amount) {
    currentHp += amount;
    if (currentHp > maxHp) currentHp = maxHp;
}

void Player::usePotion() {
    if (potions > 0 && currentHp < maxHp) {
        heal(30);
        potions--;
    }
}

void Player::gainExp(int amount) {
    currentExp += amount;
    while (currentExp >= maxExp) {
        level++;
        currentExp -= maxExp;
        maxExp = static_cast<int>(static_cast<float>(maxExp) * 1.5f);
        maxHp += 10;
        currentHp = maxHp;
        baseDamage += 2.0f;
        std::cout << "Level Up! Nivel " << level << "!\n";
    }
}

void Player::attack() {
    if (!isAttacking) {
        isAttacking = true;
        playerFrame = 0;

        static sf::Clock attackCooldown;
        if (attackCooldown.getElapsedTime().asSeconds() > 2.f) {
            std::cout << "Atacando na direcao X: " << aimDirection.x << " Y: " << aimDirection.y << "\n";

            attackCooldown.restart();
        }
    }

}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}

sf::Vector2f Player::getCenterPosition() const {
    return {sprite.getPosition().x + 32.f, sprite.getPosition().y + 32.f};
}

HeroClass Player::getClass() const {
    return this->heroClass;
}

void Player::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}
