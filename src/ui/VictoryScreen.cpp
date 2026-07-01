#include "VictoryScreen.hpp"

#include <cstdlib>
#include <sstream>
#include <vector>

VictoryScreen::VictoryScreen() : fontLoaded(false) {
}

bool VictoryScreen::loadFont() {
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
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            return true;
        }
    }

    fontLoaded = false;
    return false;
}

void VictoryScreen::draw(sf::RenderWindow& window, const Player& player, const GameState& gameState) {
    if (!fontLoaded) {
        return;
    }

    window.setView(window.getDefaultView());

    sf::RectangleShape background(sf::Vector2f(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    ));
    background.setFillColor(sf::Color(18, 30, 22)); // Slight green tint
    window.draw(background);

    window.draw(makeText("VITORIA!", 54, sf::Color(70, 230, 100), 120.f, 90.f));

    std::ostringstream summary;
    summary << "O Conde Vampiro foi derrotado!\n\n"
            << "Tempo Total: " << formatCampaignTime(gameState.campaignElapsedTime) << "\n"
            << "Pontuacao final: " << gameState.finalScore << "\n"
            << "Level alcancado: " << player.getLevel() << "\n"
            << "Inimigos derrotados: " << gameState.stats.enemiesDefeated << "\n"
            << "Itens coletados: " << gameState.stats.itemsCollected << "\n"
            << "Pocoes usadas: " << gameState.stats.potionsUsed;

    window.draw(makeText(summary.str(), 26, sf::Color::White, 130.f, 180.f));
    window.draw(makeText("R - Jogar Novamente", 24, sf::Color(255, 220, 120), 130.f, 510.f));
    window.draw(makeText("Esc - Voltar ao menu", 24, sf::Color(255, 220, 120), 130.f, 550.f));
    window.draw(makeText("Q - Sair", 24, sf::Color(255, 220, 120), 130.f, 590.f));
}

sf::Text VictoryScreen::makeText(const std::string& content, unsigned int size, sf::Color color, float x, float y) const {
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}
