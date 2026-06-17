#include "GameOverScreen.hpp"

#include <cstdlib>
#include <sstream>
#include <vector>
#include <filesystem>

GameOverScreen::GameOverScreen() : fontLoaded(false) {
}

bool GameOverScreen::loadFont() {
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

    fontLoaded = false;
    return false;
}

void GameOverScreen::draw(sf::RenderWindow& window, const Player& player, const GameState& gameState) {
    if (!fontLoaded) {
        return;
    }

    window.setView(window.getDefaultView());

    sf::RectangleShape background(sf::Vector2f(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    ));
    background.setFillColor(sf::Color(18, 22, 30));
    window.draw(background);

    window.draw(makeText("GAME OVER", 54, sf::Color(230, 70, 70), 120.f, 90.f));

    std::ostringstream summary;
    summary << "Pontuacao final: " << gameState.stats.score << "\n"
            << "Level alcancado: " << player.getLevel() << "\n"
            << "Experiencia: " << player.getCurrentExp() << "/" << player.getMaxExp() << "\n"
            << "Inimigos derrotados: " << gameState.stats.enemiesDefeated << "\n"
            << "Itens coletados: " << gameState.stats.itemsCollected << "\n"
            << "Movimentos: " << gameState.stats.movements << "\n"
            << "Dano recebido: " << gameState.stats.damageTaken << "\n"
            << "Pocoes usadas: " << gameState.stats.potionsUsed << "\n"
            << "Armadilhas ativadas: " << gameState.stats.trapsTriggered;

    window.draw(makeText(summary.str(), 26, sf::Color::White, 130.f, 180.f));
    window.draw(makeText("R - Reiniciar", 24, sf::Color(255, 220, 120), 130.f, 510.f));
    window.draw(makeText("Esc - Voltar ao menu", 24, sf::Color(255, 220, 120), 130.f, 550.f));
    window.draw(makeText("Q - Sair", 24, sf::Color(255, 220, 120), 130.f, 590.f));
}

sf::Text GameOverScreen::makeText(const std::string& content, unsigned int size, sf::Color color, float x, float y) const {
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}
