//
// Created by lucas on 11/06/2026.
//

#include "Render.hpp"
#include <vector>
#include <algorithm>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"

const float GLOBAL_SCALE = 4.0f;

struct RenderObj {
    sf::Sprite sprite;
    float yPos; // Posição de profundidade para o Y-Sorting
};
void renderFloor(RenderWindow& window, const GameState& state, Sprite& tileSprite, const MapTextures& textures) {
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            int tileType = state.currentMapGrid[y][x] % 100; // Remove flag de colisão
            
            const sf::Texture* tex = nullptr;
            if (tileType == 3) {
                // Água: Lago perfeito no começo do sprite
                tex = textures.water;
                tileSprite.setTexture(*tex);
                tileSprite.setTextureRect(sf::IntRect(0, 0, 48, 48));
                
                float scale = (float)TILE_SIZE / 48.0f;
                tileSprite.setScale(scale, scale);
                tileSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                window.draw(tileSprite);
            }
            else if (tileType == 5 || tileType == 6) {
                if (tileType == 5) tex = textures.trail;
                else tex = textures.grass;
                
                tileSprite.setTexture(*tex);
                // Miolo opaco para trilha e grama para não ficar transparente
                if (tileType == 5) {
                    tileSprite.setTextureRect(sf::IntRect(16, 16, 16, 16));
                } else {
                    tileSprite.setTextureRect(sf::IntRect(0, 64, 16, 16));
                }
                tileSprite.setScale(GLOBAL_SCALE, GLOBAL_SCALE);
                tileSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                window.draw(tileSprite);
            }
        }
    }
}

void renderObjects(RenderWindow& window, const GameState& state, Sprite& tileSprite, const MapTextures& textures, const Sprite& playerSprite) {
    std::vector<RenderObj> renderQueue;

    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            int tileType = state.currentMapGrid[y][x];
            
            // Só desenha o objeto base (ignora os blocos de colisão extra +100)
            if (tileType >= 100) continue;

            const sf::Texture* tex = nullptr;
            if (tileType == 1) tex = textures.tree;
            else if (tileType == 2) tex = textures.exit;
            else if (tileType == 4) tex = textures.wall;
            else if (tileType == 7) tex = textures.treeAutumn;
            else if (tileType == 8) tex = textures.treePine;
            else if (tileType == 9) tex = textures.bush;
            else if (tileType == 10) tex = textures.flower;
            else if (tileType == 11) tex = textures.mushroom;
            else if (tileType == 12) tex = textures.stone;

            if (tex) {
                if (tileType == 2) {
                    // Saída (Escada): Pega o frame 3 do spritesheet (pixel 96)
                    tileSprite.setTexture(*tex);
                    tileSprite.setTextureRect(sf::IntRect(96, 0, 48, 48));
                } else {
                    tileSprite.setTexture(*tex, true);
                }

                sf::Vector2u texSize = tex->getSize();
                if (tileType == 2) {
                    texSize = sf::Vector2u(48, 48); // Tamanho forçado do IntRect
                }
                
                float scale = 1.0f;
                if (tileType == 4) {
                    scale = 2.0f; // Montanha grande!
                } else if (tileType == 1 || tileType == 7 || tileType == 8 || (tileType >= 9 && tileType <= 12)) {
                    scale = 2.0f;
                } else {
                    scale = (float)TILE_SIZE / texSize.x;
                }
                
                tileSprite.setScale(scale, scale);
                
                float scaledWidth = texSize.x * scale;
                float scaledHeight = texSize.y * scale;
                
                float baseY = y * TILE_SIZE + TILE_SIZE; // Base da célula
                float drawX = x * TILE_SIZE + (TILE_SIZE / 2.0f) - (scaledWidth / 2.0f);
                
                if (tileType == 4) {
                    // Montanha alinha perfeitamente à esquerda para cobrir os blocos de colisão certinhos
                    drawX = x * TILE_SIZE;
                }
                
                tileSprite.setPosition(drawX, baseY - scaledHeight);
                tileSprite.setColor(sf::Color(255, 255, 255, 255));
                
                renderQueue.push_back(RenderObj{tileSprite, baseY});
            }
        }
    }

    // Adiciona o jogador na fila
    sf::FloatRect bounds = playerSprite.getGlobalBounds();
    float playerBaseY = bounds.top + bounds.height; // A base dos pés do jogador
    
    // Precisamos de uma cópia modificável do player para a fila
    sf::Sprite pSprite = playerSprite;
    renderQueue.push_back(RenderObj{pSprite, playerBaseY});

    // Ordena pelo eixo Y (Y-Sorting)
    std::sort(renderQueue.begin(), renderQueue.end(), [](const RenderObj& a, const RenderObj& b) {
        return a.yPos < b.yPos;
    });

    // Desenha tudo na ordem correta
    for (auto& obj : renderQueue) {
        // Se o objeto estiver na frente do jogador E se sobrepuser à área visual do jogador, aplicamos transparência!
        if (obj.yPos > playerBaseY) {
            if (obj.sprite.getGlobalBounds().intersects(playerSprite.getGlobalBounds())) {
                obj.sprite.setColor(sf::Color(255, 255, 255, 140)); // Fica 45% transparente!
            }
        }
        window.draw(obj.sprite);
    }
}
