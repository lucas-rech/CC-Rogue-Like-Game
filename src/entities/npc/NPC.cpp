#include "NPC.hpp"
#include "../hero/Player.hpp"
#include <cmath>

NPC::NPC(sf::Vector2f position) 
    : currentFrame(0), frameCount(4), frameWidth(64), frameHeight(64), 
      animationTimer(0.f), showDialog(false) {
    sprite.setPosition(position);
    
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "assets/fonts/DejaVuSans.ttf",
        "assets/fonts/OpenSans-Regular.ttf",
        "assets/fonts/Roboto-Regular.ttf"
    };

    if (const char* windowsDir = std::getenv("WINDIR")) {
        fontPaths.emplace_back(std::string(windowsDir) + "/Fonts/arial.ttf");
    }

    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
    
    dialogText.setFont(font);
    setDialog("...");
    dialogText.setCharacterSize(9);
    dialogText.setFillColor(sf::Color::White);
    
    dialogBg.setFillColor(sf::Color(0, 0, 0, 180));
    dialogBg.setOutlineThickness(1.f);
    dialogBg.setOutlineColor(sf::Color::White);
    
    sf::FloatRect textBounds = dialogText.getLocalBounds();
    dialogBg.setSize(sf::Vector2f(textBounds.width + 10.f, textBounds.height + 10.f));
    
    dialogBg.setPosition(position.x - dialogBg.getSize().x / 2.f + 32.f, position.y - 40.f);
    dialogText.setPosition(dialogBg.getPosition().x + 5.f, dialogBg.getPosition().y + 5.f);
}

bool NPC::loadTexture(const std::string& path) {
    if (idleTexture.loadFromFile(path)) {
        sprite.setTexture(idleTexture);
        sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        return true;
    }
    return false;
}

void NPC::update(float deltaTime, const Player& player) {
    animationTimer += deltaTime;
    if (animationTimer >= 0.12f) {
        currentFrame = (currentFrame + 1) % frameCount;
        sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
        animationTimer = 0.f;
    }
    
    sf::Vector2f playerPos = player.getCenterPosition();
    sf::Vector2f npcPos(sprite.getPosition().x + 32.f, sprite.getPosition().y + 32.f);
    float dx = playerPos.x - npcPos.x;
    float dy = playerPos.y - npcPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    showDialog = (distance < 100.f);
}

void NPC::setDialog(const std::string& text) {
    dialogText.setString(text);
    sf::FloatRect textBounds = dialogText.getLocalBounds();
    dialogBg.setSize(sf::Vector2f(textBounds.width + 10.f, textBounds.height + 10.f));
    dialogBg.setPosition(sprite.getPosition().x - dialogBg.getSize().x / 2.f + 32.f, sprite.getPosition().y - 40.f);
    dialogText.setPosition(dialogBg.getPosition().x + 5.f, dialogBg.getPosition().y + 5.f);
}

void NPC::setFrameProperties(int count, int width, int height) {
    frameCount = count;
    frameWidth = width;
    frameHeight = height;
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
}

void NPC::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(sprite, states);
}

void NPC::drawDialog(sf::RenderTarget& target, sf::RenderStates states) const {
    if (showDialog) {
        target.draw(dialogBg, states);
        target.draw(dialogText, states);
    }
}
