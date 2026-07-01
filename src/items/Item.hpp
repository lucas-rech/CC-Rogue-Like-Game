#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "../entities/hero/Player.hpp"
class Enemy;
#include "../world/Level.hpp"

enum class ItemType {
    Potion,
    Key,
    Weapon,
    Shield,
    PowerUp,
    ManaPotion,
    SpellTome
};

class Item : public sf::Drawable {
public:
    Item(ItemType type, sf::Vector2f position);

    sf::FloatRect getBounds() const;
    bool isCollected() const;
    void collect(Player& player, GameStats& stats, int currentLevel);
    void destroySilent(); // Usado pelos monstros para destruir itens (sem dar pontos pro jogador)
    ItemType getType() const { return type; }

private:
    ItemType type;
    bool collected;
    std::string name;
    std::string description;
    int value;
    sf::RectangleShape shape;
    sf::Sprite sprite;
    sf::Texture texture;
    bool hasSprite;

    void setupItem();
    void applyEffect(Player& player);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

void spawnItems(std::vector<Item>& items, const GameState& gameState, const Player& player);
void updateItems(std::vector<Item>& items, Player& player, GameState& gameState, const std::vector<Enemy>& enemies);
void drawItems(sf::RenderWindow& window, const std::vector<Item>& items);
