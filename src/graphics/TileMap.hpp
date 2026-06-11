#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

struct TilesetInfo {
    int firstGid;
    sf::Texture texture;
    int columns;
    int tileCount;
};

struct TileLayer {
    std::string name;
    std::vector<int> data;
    // Um VertexArray por Tileset (para contornar a limitação de 1 textura por VertexArray no SFML)
    std::map<int, sf::VertexArray> verticesPerTileset;
    bool visible;
};

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    TileMap();
    ~TileMap() = default;

    bool loadFromJson(const std::string& filename);

    // Retorna true se a posição x,y colidir com algum bloco sólido
    bool checkCollision(float worldX, float worldY) const;

    // Desenha as camadas normais (chão, objetos)
    void drawAll(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;
    
    // Desenha as camadas que ficam na frente do jogador (folhas, telhados)
    void drawForeground(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;

    // Renderiza as camadas
    void drawLayer(sf::RenderTarget& target, sf::RenderStates states, const std::string& layerName) const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getTileSize() const { return tileSize; }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void buildVertices(TileLayer& layer);
    int findTilesetIndex(int tileId) const;

    std::vector<TileLayer> layers;
    std::vector<TilesetInfo> tilesets;
    int width;
    int height;
    int tileSize;
};
