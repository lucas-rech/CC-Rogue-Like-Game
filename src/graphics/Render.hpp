#pragma once
#pragma once

#include <SFML/Graphics.hpp>
#include "../world/Level.hpp"

using namespace sf;

struct MapTextures {
    const sf::Texture* grass;
    const sf::Texture* tree;
    const sf::Texture* treeAutumn;
    const sf::Texture* treePine;
    const sf::Texture* bush;
    const sf::Texture* flower;
    const sf::Texture* mushroom;
    const sf::Texture* stone;
    const sf::Texture* exit;
    const sf::Texture* water;
    const sf::Texture* wall;
    const sf::Texture* trail;
};

void renderFloor(RenderWindow& window, const GameState& state, Sprite& tileSprite, const MapTextures& textures);
void renderObjects(RenderWindow& window, const GameState& state, Sprite& tileSprite, const MapTextures& textures, const Sprite& playerSprite);
