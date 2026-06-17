#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class GameScreen {
    Menu,
    Playing,
    HowToPlay,
    ItemsInfo,
    ScoreInfo,
    GameOver,
    Victory,
    Exit
};

class Menu {
public:
    Menu();

    bool loadFont();
    GameScreen handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window, GameScreen screen);

private:
    sf::Font font;
    bool fontLoaded;
    int selectedOption;
    std::vector<std::string> options;

    void moveSelection(int direction);
    GameScreen selectCurrentOption() const;
    void drawMainMenu(sf::RenderWindow& window);
    void drawInfoScreen(sf::RenderWindow& window, const std::string& title, const std::vector<std::string>& lines);
    sf::Text makeText(const std::string& content, unsigned int size, sf::Color color, float x, float y) const;
};
