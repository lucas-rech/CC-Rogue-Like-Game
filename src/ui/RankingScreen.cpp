#include "RankingScreen.hpp"
#include "../system/Ranking.hpp"
#include <cstdlib>
#include <filesystem>
#include <sstream>

RankingScreen::RankingScreen() : fontLoaded(false) {}

bool RankingScreen::loadFont() {
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "assets/fonts/DejaVuSans.ttf",
        "assets/fonts/OpenSans-Regular.ttf",
        "assets/fonts/Roboto-Regular.ttf"
    };

    if (const char* windowsDir = std::getenv("WINDIR")) {
        fontPaths.emplace_back(std::string(windowsDir) + "/Fonts/arial.ttf");
    }

    for (const auto& path : fontPaths) {
        if (std::filesystem::exists(path) && font.loadFromFile(path)) {
            fontLoaded = true;
            return true;
        }
    }
    return false;
}

void RankingScreen::draw(sf::RenderWindow& window) {
    if (!fontLoaded) return;

    window.setView(window.getDefaultView());

    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(sf::Color(18, 22, 30));
    window.draw(background);

    window.draw(makeText("RANKING - Top Pontuacoes", 44, sf::Color(255, 220, 120), 120.f, 60.f));

    std::vector<RankingEntry> entries = Ranking::loadScores();

    float y = 140.f;
    window.draw(makeText("NOME           DIFICULDADE   TEMPO     SCORE", 24, sf::Color(150, 150, 150), 120.f, y));
    y += 40.f;

    int limit = std::min((int)entries.size(), 10);
    for (int i = 0; i < limit; i++) {
        std::ostringstream line;
        // Basic spacing for table feel
        line << entries[i].playerName;
        while(line.str().length() < 15) line << " ";
        line << entries[i].difficulty;
        while(line.str().length() < 29) line << " ";
        line << entries[i].timeFormatted;
        while(line.str().length() < 39) line << " ";
        line << entries[i].score;
        
        window.draw(makeText(line.str(), 22, sf::Color::White, 120.f, y));
        y += 35.f;
    }

    if (entries.empty()) {
        window.draw(makeText("Nenhum registro encontrado.", 22, sf::Color::White, 120.f, y));
    }

    window.draw(makeText("Pressione Esc para voltar ao menu", 20, sf::Color(190, 195, 205), 120.f, 600.f));
}

sf::Text RankingScreen::makeText(const std::string& content, unsigned int size, sf::Color color, float x, float y) const {
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}
