#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Player;

class NPC : public sf::Drawable {
public:
    NPC(sf::Vector2f position);
    bool loadTexture(const std::string& path);
    void setDialog(const std::string& text);
    void setFrameProperties(int count, int width, int height);
    void update(float deltaTime, const Player& player);
    void drawDialog(sf::RenderTarget& target, sf::RenderStates states) const;
    
private:
    sf::Sprite sprite;
    sf::Texture idleTexture;
    int currentFrame;
    int frameCount;
    int frameWidth;
    int frameHeight;
    float animationTimer;
    bool showDialog;
    
    sf::Font font;
    sf::Text dialogText;
    sf::RectangleShape dialogBg;
    
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
