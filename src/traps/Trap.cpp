#include "Trap.hpp"
#include <random>

namespace {
    std::mt19937& trapRandomGenerator() {
        static std::mt19937 generator(std::random_device{}());
        return generator;
    }
    
    sf::Texture globalTrapTexture;
    bool globalTrapTextureLoaded = false;
}

#include <cmath>

namespace {
    float distanceBetween(sf::Vector2f a, sf::Vector2f b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    bool isValidTrapSpawn(sf::Vector2f position, const GameState& gameState, const Player& player) {
        sf::FloatRect trapArea(position.x, position.y, 16.f, 16.f);
        return !checkCollision(trapArea, gameState)
            && distanceBetween(position, player.getPosition()) > 120.f;
    }
}

Trap::Trap(TrapType type, sf::Vector2f position)
    : type(type), damage(0), active(true), cooldown(1.f), timer(0.f), currentFrame(0), animationTimer(0.f), isAnimating(false) {
    shape.setSize({16.f, 16.f});
    shape.setPosition(position);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color(20, 20, 20));

    if (!globalTrapTextureLoaded) {
        if (globalTrapTexture.loadFromFile("assets/textures/trap.png")) {
            globalTrapTextureLoaded = true;
        }
    }

    if (globalTrapTextureLoaded) {
        sprite.setTexture(globalTrapTexture);
        int frameW = globalTrapTexture.getSize().x / 4;
        int frameH = globalTrapTexture.getSize().y;
        sprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
        sprite.setPosition(position);
        
        float scaleX = 16.f / static_cast<float>(frameW);
        float scaleY = 16.f / static_cast<float>(frameH);
        sprite.setScale(scaleX, scaleY);
    }

    setupTrap();
}

void Trap::update(float deltaTime, Player& player, GameState& gameState) {
    if (timer > 0.f) {
        timer -= deltaTime;
        if (timer <= 0.f) {
            active = true;
        }
    }

    if (isAnimating) {
        animationTimer += deltaTime;
        if (animationTimer >= 0.15f) {
            animationTimer = 0.f;
            currentFrame++;
            if (currentFrame >= 4) {
                currentFrame = 0;
                isAnimating = false;
            }
            if (globalTrapTextureLoaded) {
                int frameW = globalTrapTexture.getSize().x / 4;
                sprite.setTextureRect(sf::IntRect(currentFrame * frameW, 0, frameW, globalTrapTexture.getSize().y));
            }
        }
    }

    if (active && getBounds().intersects(player.getHitbox())) {
        applyEffect(player, gameState);
        timer = cooldown;
        active = false;
        isAnimating = true;
        currentFrame = 1;
        if (globalTrapTextureLoaded) {
            int frameW = globalTrapTexture.getSize().x / 4;
            sprite.setTextureRect(sf::IntRect(currentFrame * frameW, 0, frameW, globalTrapTexture.getSize().y));
        }
    }
}

sf::FloatRect Trap::getBounds() const {
    return shape.getGlobalBounds();
}

void Trap::setupTrap() {
    switch (type) {
        case TrapType::Spikes:
            damage = 10;
            cooldown = 1.0f;
            shape.setFillColor(sf::Color(120, 120, 125));
            break;
        case TrapType::Fire:
            damage = 15;
            cooldown = 1.5f;
            shape.setFillColor(sf::Color(230, 90, 35));
            break;
    }
}

void Trap::applyEffect(Player& player, GameState& gameState) {
    int actualDamage = player.takeDamage(damage);
    gameState.stats.registerTrapTriggered(gameState.currentLevel);
    gameState.stats.registerDamageTaken(actualDamage, gameState.currentLevel);

    if (player.getCurrentHp() <= 0) {
        gameState.isGameOver = true;
    }
}

void Trap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (globalTrapTextureLoaded) {
        target.draw(sprite, states);
    } else {
        target.draw(shape, states);
    }
}

void spawnTraps(std::vector<Trap>& traps, const GameState& gameState, const Player& player) {
    traps.clear();
    traps.reserve(30); // Let's spawn 30 traps

    std::uniform_int_distribution<int> xDist(2, gameState.map.getWidth() - 3);
    std::uniform_int_distribution<int> yDist(2, gameState.map.getHeight() - 3);

    for (int i = 0; i < 30; ++i) {
        bool spawned = false;
        int attempts = 0;
        while (!spawned && attempts < 50) {
            int x = xDist(trapRandomGenerator());
            int y = yDist(trapRandomGenerator());
            sf::Vector2f pos(static_cast<float>(x * gameState.map.getTileSize()),
                             static_cast<float>(y * gameState.map.getTileSize()));

            if (isValidTrapSpawn(pos, gameState, player)) {
                TrapType type = trapRandomGenerator()() % 2 == 0 ? TrapType::Spikes : TrapType::Fire;
                traps.emplace_back(type, pos);
                spawned = true;
            }
            attempts++;
        }
    }
}

void updateTraps(std::vector<Trap>& traps, Player& player, GameState& gameState, float deltaTime) {
    for (Trap& trap : traps) {
        trap.update(deltaTime, player, gameState);
    }
}

void drawTraps(sf::RenderWindow& window, const std::vector<Trap>& traps) {
    for (const Trap& trap : traps) {
        window.draw(trap);
    }
}
