#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "../entities/hero/Player.hpp"
#include "../world/Level.hpp"

enum class TrapType {
    Spikes,
    Fire
};

class Trap : public sf::Drawable {
public:
    Trap(TrapType type, sf::Vector2f position);

    void update(float deltaTime, Player& player, GameState& gameState);
    sf::FloatRect getBounds() const;

private:
    TrapType type;
    int damage;
    bool active;
    float cooldown;
    float timer;
    sf::RectangleShape shape;
    sf::Sprite sprite;

    int currentFrame;
    float animationTimer;
    bool isAnimating;

    void setupTrap();
    void applyEffect(Player& player, GameState& gameState);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

void spawnTraps(std::vector<Trap>& traps, const GameState& gameState, const Player& player);
void updateTraps(std::vector<Trap>& traps, Player& player, GameState& gameState, float deltaTime);
void drawTraps(sf::RenderWindow& window, const std::vector<Trap>& traps);
