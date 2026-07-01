#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class RankingScreen {
public:
    RankingScreen();
    bool loadFont();
    void draw(sf::RenderWindow& window);

private:
    sf::Font font;
    bool fontLoaded;
    sf::Text makeText(const std::string& content, unsigned int size, sf::Color color, float x, float y) const;
};
