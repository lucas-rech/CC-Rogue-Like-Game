#include "HUD.hpp"

#include <cstdlib>
#include <sstream>
#include <vector>

HUD::HUD() : fontLoaded(false) {
}

bool HUD::loadFont() {
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

void HUD::draw(sf::RenderWindow& window, const Player& player, const GameState& gameState) {
    if (!fontLoaded) {
        return;
    }

    sf::View oldView = window.getView();
    window.setView(window.getDefaultView());

    sf::RectangleShape background(sf::Vector2f(340.f, 370.f));
    background.setPosition(12.f, 12.f);
    background.setFillColor(sf::Color(0, 0, 0, 150));
    background.setOutlineThickness(1.f);
    background.setOutlineColor(sf::Color(255, 255, 255, 80));
    window.draw(background);

    if (player.getUnspentPoints() > 0) {
        sf::Text attrText = makeText("Pontos de Atributo: " + std::to_string(player.getUnspentPoints()) + " (Aperte I)", 320.f, 15.f);
        attrText.setFillColor(sf::Color(255, 215, 0)); // Gold color
        window.draw(attrText);
    }

    std::ostringstream text;
    text << "Jogador: " << gameState.playerName << "\n"
         << "Vida: " << player.getCurrentHp() << "/" << player.getMaxHp() << "\n"
         << "Level: " << player.getLevel() << "\n"
         << "XP: " << player.getCurrentExp() << "/" << player.getMaxExp() << "\n"
         << "Mana: " << player.getCurrentMana() << "/" << player.getMaxMana() << "\n"
         << "Pocoes: " << player.getPotions() << "\n"
         << "Pocoes de Mana: " << player.getManaPotions() << "\n"
         << "Chaves: " << player.getKeys() << "\n"
         << "Armas Coletadas: " << player.getWeaponsCollected() << "\n"
         << "Escudos Coletados: " << player.getShieldsCollected() << "\n"
         << "Power-ups: " << player.getPowerUpsCollected() << "\n"
         << "Dano: " << player.getDamage() << "\n"
         << "Armadura: " << player.getArmor() << "\n"
         << "Pontuacao: " << gameState.campaignScore << "\n"
         << "Tempo: " << formatCampaignTime(gameState.campaignElapsedTime) << "\n"
         << "Fase: " << gameState.currentLevel << "/" << gameState.maxLevel << "\n"
         << "Dificuldade: " << getDifficultyName(gameState.difficulty);

    window.draw(makeText(text.str(), 24.f, 22.f));
    window.setView(oldView);
}

sf::Text HUD::makeText(const std::string& content, float x, float y) const {
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    text.setPosition(x, y);
    return text;
}
