#include "Companion.hpp"
#include <cmath>
#include <iostream>

Companion::Companion(float startX, float startY) 
    : speed(60.f), frameWidth(16), frameHeight(16), 
      textureRow(0), currentFrame(0), totalFramesPerRow(6), 
      animationTimer(0.f), animationSpeed(0.15f) {
    sprite.setPosition(startX, startY);
    sprite.setScale(1.5f, 1.5f);
}

bool Companion::loadTexture() {
    if (texture.loadFromFile("assets/textures/characters/pet/chicken.png")) {
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        return true;
    }
    std::cout << "ERRO: Nao foi possivel carregar a textura do pet em assets/textures/characters/pet/chicken.png\n";
    return false;
}

void Companion::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void Companion::update(float deltaTime, const Player& player, std::vector<Item>& items) {
    sf::Vector2f targetPos = player.getCenterPosition();
    float minDistanceToItem = 200.f; // The pet's vision range
    Item* targetItem = nullptr;

    float distToPlayer = std::sqrt(std::pow(sprite.getPosition().x - targetPos.x, 2) + 
                                   std::pow(sprite.getPosition().y - targetPos.y, 2));

    // Look for nearby uncollected items only if the player is not too far away
    if (distToPlayer < 350.f) {
        for (Item& item : items) {
            if (!item.isCollected()) {
                sf::FloatRect bounds = item.getBounds();
                sf::Vector2f itemCenter(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
                
                float dist = std::sqrt(std::pow(sprite.getPosition().x - itemCenter.x, 2) + 
                                       std::pow(sprite.getPosition().y - itemCenter.y, 2));
                                       
                if (dist < minDistanceToItem) {
                    minDistanceToItem = dist;
                    targetItem = &item;
                }
            }
        }
    }

    if (targetItem) {
        sf::FloatRect bounds = targetItem->getBounds();
        targetPos = sf::Vector2f(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        
        // If close enough, "collect" it. (Destroy the item and give player a small visual prompt or just destroy it so player doesn't have to walk).
        // Wait, the user said "buscará por itens próximos". I assume the pet collects them for the player? 
        // No, let's just make it move towards them. But it would get stuck on the item! 
        // Let's make the pet push the item towards the player, or just collect it for the player.
        // I can't call `targetItem->collect(player...)` easily here without modifying the signature to pass GameState.
        // So the pet just hovers over the item, showing the player where it is.
    }

    sf::Vector2f currentPos = sprite.getPosition();
    sf::Vector2f dir = targetPos - currentPos;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    bool isMoving = false;
    
    // Don't overlap the player exactly, keep a distance of 40 unless targeting an item
    float stopDistance = targetItem ? 5.f : 40.f;

    if (dist > stopDistance) {
        dir /= dist; // Normalize
        velocity = dir * speed * deltaTime;
        sprite.move(velocity);
        isMoving = true;

        // Determine animation row (0=down, 1=up, 2=left, 3=right)
        if (std::abs(dir.x) > std::abs(dir.y)) {
            textureRow = (dir.x > 0) ? 3 : 2; // Right or Left
        } else {
            textureRow = (dir.y > 0) ? 0 : 1; // Down or Up
        }
    }

    if (isMoving) {
        animationTimer += deltaTime;
        if (animationTimer >= animationSpeed) {
            animationTimer = 0.f;
            currentFrame = (currentFrame + 1) % totalFramesPerRow;
        }
    } else {
        currentFrame = 0; // Idle frame
    }

    sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, textureRow * frameHeight, frameWidth, frameHeight));
}

void Companion::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(sprite, states);
}
