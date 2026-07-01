#include "Item.hpp"
#include "../entities/enemies/Enemy.hpp"

#include <cmath>
#include <random>

namespace {
    std::mt19937& itemRandomGenerator() {
        static std::mt19937 generator(std::random_device{}());
        return generator;
    }
    float distanceBetween(sf::Vector2f a, sf::Vector2f b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    bool isValidItemSpawn(sf::Vector2f position, const GameState& gameState, const Player& player) {
        sf::FloatRect itemArea(position.x, position.y, 14.f, 14.f);
        return !checkCollision(itemArea, gameState)
            && distanceBetween(position, player.getPosition()) > 80.f;
    }
}

Item::Item(ItemType type, sf::Vector2f position)
    : type(type), collected(false), value(0), hasSprite(false) {
    shape.setSize({14.f, 14.f});
    shape.setPosition(position);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color::White);
    setupItem();
    if (hasSprite) {
        sprite.setPosition(position);
    }
}

sf::FloatRect Item::getBounds() const {
    if (hasSprite) {
        return sprite.getGlobalBounds();
    }
    return shape.getGlobalBounds();
}

bool Item::isCollected() const {
    return collected;
}

void Item::collect(Player& player, GameStats& stats, int currentLevel) {
    if (collected) {
        return;
    }

    collected = true;
    applyEffect(player);
    stats.registerItemCollected(value, currentLevel);
}

void Item::destroySilent() {
    collected = true;
}

void Item::setupItem() {
    switch (type) {
        case ItemType::Potion:
            name = "Pocao de vida";
            description = "Adiciona uma pocao ao inventario.";
            value = 30;
            if (texture.loadFromFile("assets/textures/items/PNG/Transperent/life.png")) {
                sprite.setTexture(texture);
                float scaleX = 14.f / texture.getSize().x;
                float scaleY = 14.f / texture.getSize().y;
                sprite.setScale(scaleX, scaleY);
                hasSprite = true;
            } else {
                shape.setFillColor(sf::Color(210, 50, 70));
            }
            break;
        case ItemType::Key:
            name = "Chave";
            description = "Prepara o jogador para abrir portas.";
            value = 50;
            if (texture.loadFromFile("assets/textures/key.png")) {
                sprite.setTexture(texture);
                float scaleX = 14.f / texture.getSize().x;
                float scaleY = 14.f / texture.getSize().y;
                sprite.setScale(scaleX, scaleY);
                hasSprite = true;
            } else {
                shape.setFillColor(sf::Color(240, 210, 70));
            }
            break;
        case ItemType::Weapon:
            name = "Arma";
            description = "Aumenta o dano do jogador.";
            value = 75;
            shape.setFillColor(sf::Color(150, 150, 160));
            break;
        case ItemType::Shield:
            name = "Escudo";
            description = "Aumenta a armadura do jogador permanentemente.";
            value = 75;
            if (texture.loadFromFile("src/assets/textures/items/PNG/Transperent/Icon32.png")) {
                sprite.setTexture(texture);
                float scaleX = 14.f / texture.getSize().x;
                float scaleY = 14.f / texture.getSize().y;
                sprite.setScale(scaleX, scaleY);
                hasSprite = true;
            } else {
                shape.setFillColor(sf::Color(70, 120, 220));
            }
            break;
        case ItemType::PowerUp:
            name = "Power-up";
            description = "Aumenta a vida maxima do jogador.";
            value = 100;
            shape.setFillColor(sf::Color(130, 80, 220));
            break;
        case ItemType::ManaPotion:
            name = "Pocao de mana";
            description = "Adiciona uma pocao de mana ao inventario.";
            value = 30;
            // Let's use Icon46.png for mana (blue potion usually) or just color if not found
            if (texture.loadFromFile("assets/textures/items/PNG/Transperent/Icon46.png")) {
                sprite.setTexture(texture);
                float scaleX = 14.f / texture.getSize().x;
                float scaleY = 14.f / texture.getSize().y;
                sprite.setScale(scaleX, scaleY);
                hasSprite = true;
            } else {
                shape.setFillColor(sf::Color(50, 70, 210)); // Blue
            }
            break;
        case ItemType::SpellTome:
            name = "Livro de Magia";
            description = "Desbloqueia a habilidade de atirar magia com o botao direito.";
            value = 500;
            if (texture.loadFromFile("assets/textures/items/PNG/Transperent/Icon31.png")) {
                sprite.setTexture(texture);
                float scaleX = 14.f / texture.getSize().x;
                float scaleY = 14.f / texture.getSize().y;
                sprite.setScale(scaleX, scaleY);
                hasSprite = true;
            } else {
                shape.setFillColor(sf::Color(160, 32, 240)); // Purple
            }
            break;
    }
}

void Item::applyEffect(Player& player) {
    switch (type) {
        case ItemType::Potion:
            player.addPotion();
            break;
        case ItemType::ManaPotion:
            player.addManaPotion();
            break;
        case ItemType::SpellTome:
            player.unlockMagic();
            break;
        case ItemType::Key:
            player.addKey();
            break;
        case ItemType::Weapon:
            player.increaseDamage(5);
            break;
        case ItemType::Shield:
            player.increaseArmor(2);
            break;
        case ItemType::PowerUp:
            player.increaseMaxHp(15);
            break;
    }
}

void Item::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!collected) {
        if (hasSprite) {
            target.draw(sprite, states);
        } else {
            target.draw(shape, states);
        }
    }
}

void spawnItems(std::vector<Item>& items, const GameState& gameState, const Player& player) {
    items.clear();
    const DifficultyConfig& config = getDifficultyConfig(gameState.difficulty);
    items.reserve(50);

    // Fixed spawns
    std::vector<std::pair<ItemType, sf::Vector2f>> fixedSpawns = {
        {ItemType::Key,       {68.29f, 755.65f}},
        {ItemType::Key,       {198.65f, 1195.3f}},
        {ItemType::Key,       {3064.55f, 928.601f}},
        {ItemType::Key,       {202.7f, 1426.15f}},
        {ItemType::SpellTome, {845.f, 938.5f}},
        {ItemType::PowerUp,   {3160.f, 250.f}} // Item Secreto!
    };

    for (const auto& spawn : fixedSpawns) {
        items.emplace_back(spawn.first, spawn.second);
    }

    // Random spawns
    std::uniform_int_distribution<int> xDist(2, gameState.map.getWidth() - 3);
    std::uniform_int_distribution<int> yDist(2, gameState.map.getHeight() - 3);

    auto spawnRandomItem = [&](ItemType type, int count) {
        for (int i = 0; i < count; ++i) {
            bool spawned = false;
            int attempts = 0;
            while (!spawned && attempts < 50) {
                int x = xDist(itemRandomGenerator());
                int y = yDist(itemRandomGenerator());
                sf::Vector2f pos(static_cast<float>(x * gameState.map.getTileSize()),
                                 static_cast<float>(y * gameState.map.getTileSize()));

                bool farFromOtherItems = true;
                for (const Item& item : items) {
                    if (distanceBetween(pos, {item.getBounds().left, item.getBounds().top}) < 24.f) {
                        farFromOtherItems = false;
                        break;
                    }
                }

                if (farFromOtherItems && isValidItemSpawn(pos, gameState, player)) {
                    items.emplace_back(type, pos);
                    spawned = true;
                }
                attempts++;
            }
        }
    };

    spawnRandomItem(ItemType::Potion, config.healthPotionCount);
    spawnRandomItem(ItemType::ManaPotion, config.manaPotionCount);
    spawnRandomItem(ItemType::Shield, config.shieldCount);
    spawnRandomItem(ItemType::Weapon, 2);
    spawnRandomItem(ItemType::PowerUp, 2);
}

void updateItems(std::vector<Item>& items, Player& player, GameState& gameState, const std::vector<Enemy>& enemies) {
    sf::FloatRect playerHitbox = player.getHitbox();

    for (Item& item : items) {
        if (!item.isCollected()) {
            if (playerHitbox.intersects(item.getBounds())) {
                item.collect(player, gameState.stats, gameState.currentLevel);
                gameState.campaignScore += 50;
                continue;
            }

            // Inimigos destroem os itens menores apenas
            if (item.getType() != ItemType::Key && item.getType() != ItemType::PowerUp && item.getType() != ItemType::SpellTome) {
                for (const Enemy& enemy : enemies) {
                    if (enemy.isAlive() && enemy.getHitbox().intersects(item.getBounds())) {
                        item.destroySilent();
                        break;
                    }
                }
            }
        }
    }
}

void drawItems(sf::RenderWindow& window, const std::vector<Item>& items) {
    for (const Item& item : items) {
        if (!item.isCollected()) {
            window.draw(item);
        }
    }
}
