#include "AttributeMenu.hpp"
#include <cstdlib>
#include <vector>

AttributeMenu::AttributeMenu() : fontLoaded(false) {}

bool AttributeMenu::loadFont() {
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
    return false;
}

void AttributeMenu::draw(sf::RenderWindow& window, const Player& player) {
    if (!fontLoaded) return;

    sf::View oldView = window.getView();
    window.setView(window.getDefaultView());

    // Fundo semitransparente escuro
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    // Caixa do menu
    sf::RectangleShape bgBox(sf::Vector2f(400.f, 300.f));
    bgBox.setPosition(120.f, 100.f);
    bgBox.setFillColor(sf::Color(30, 30, 40));
    bgBox.setOutlineThickness(2.f);
    bgBox.setOutlineColor(sf::Color(200, 180, 50));
    window.draw(bgBox);

    window.draw(makeText("DISTRIBUICAO DE ATRIBUTOS", 24, sf::Color(255, 215, 0), 140.f, 110.f));
    
    std::string pointsText = "Pontos Disponiveis: " + std::to_string(player.getUnspentPoints());
    window.draw(makeText(pointsText, 20, sf::Color::White, 140.f, 150.f));

    if (player.getUnspentPoints() > 0) {
        window.draw(makeText("[1] + Vida (Cura e +MaxHP)", 20, sf::Color(100, 255, 100), 150.f, 200.f));
        window.draw(makeText("[2] + Dano (+Base Damage)", 20, sf::Color(255, 100, 100), 150.f, 240.f));
        window.draw(makeText("[3] + Velocidade (+0.2 Speed)", 20, sf::Color(100, 100, 255), 150.f, 280.f));
    } else {
        window.draw(makeText("Voce nao tem pontos.", 20, sf::Color(150, 150, 150), 150.f, 240.f));
    }

    window.draw(makeText("Pressione 'I' ou Esc para fechar", 16, sf::Color(200, 200, 200), 150.f, 360.f));

    window.setView(oldView);
}

sf::Text AttributeMenu::makeText(const std::string& content, unsigned int size, sf::Color color, float x, float y) const {
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}
