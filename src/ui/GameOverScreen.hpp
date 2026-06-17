#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "../entities/hero/Player.hpp"
#include "../world/Level.hpp"

class GameOverScreen {
public:
    GameOverScreen();

    bool loadFont();
    void draw(sf::RenderWindow& window, const Player& player, const GameState& gameState);

private:
    sf::Font font;
    bool fontLoaded;

    sf::Text makeText(const std::string& content, unsigned int size, sf::Color color, float x, float y) const;
};
