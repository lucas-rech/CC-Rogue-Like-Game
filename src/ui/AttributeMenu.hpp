#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "../entities/hero/Player.hpp"

class AttributeMenu {
public:
    AttributeMenu();

    bool loadFont();
    void draw(sf::RenderWindow& window, const Player& player);

private:
    sf::Font font;
    bool fontLoaded;

    sf::Text makeText(const std::string& content, unsigned int size, sf::Color color, float x, float y) const;
};
