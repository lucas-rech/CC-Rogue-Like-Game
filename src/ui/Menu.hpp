#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "../world/Level.hpp"

enum class GameScreen {
    Menu,
    PlayerName,
    DifficultySelect,
    Playing,
    HowToPlay,
    ItemsInfo,
    ScoreInfo,
    Ranking,
    GameOver,
    Victory,
    Exit
};

class Menu {
public:
    Menu();

    bool loadFont();
    GameScreen handleEvent(const sf::Event& event, GameScreen screen);
    Difficulty getSelectedDifficulty() const;
    const std::string& getPlayerName() const;
    void draw(sf::RenderWindow& window, GameScreen screen);

private:
    sf::Font font;
    bool fontLoaded;
    int selectedOption;
    Difficulty selectedDifficulty;
    std::string playerName;
    std::vector<std::string> mainOptions;
    std::vector<std::string> difficultyOptions;

    void moveSelection(int direction, int optionCount);
    GameScreen selectMainOption();
    GameScreen selectDifficultyOption();
    GameScreen handleNameInput(const sf::Event& event);
    void drawMainMenu(sf::RenderWindow& window);
    void drawNameInput(sf::RenderWindow& window);
    void drawDifficultyMenu(sf::RenderWindow& window);
    void drawInfoScreen(sf::RenderWindow& window, const std::string& title, const std::vector<std::string>& lines);
    sf::Text makeText(const std::string& content, unsigned int size, sf::Color color, float x, float y) const;
};