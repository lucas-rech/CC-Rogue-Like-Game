#pragma once
#include <SFML/Graphics.hpp>
#include "../hero/Player.hpp"
#include "../../items/Item.hpp"
#include <vector>

class Companion : public sf::Drawable {
public:
    Companion(float startX, float startY);
    
    bool loadTexture();
    void update(float deltaTime, const Player& player, std::vector<Item>& items);
    void setPosition(float x, float y);
    
private:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f velocity;
    
    float speed;
    int frameWidth;
    int frameHeight;
    int textureRow;
    int currentFrame;
    int totalFramesPerRow;
    float animationTimer;
    float animationSpeed;
    
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
