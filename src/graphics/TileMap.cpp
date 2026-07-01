#include "TileMap.hpp"
#include <fstream>
#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

TileMap::TileMap() : width(0), height(0), tileSize(16) {}

bool TileMap::loadFromJson(const std::string& filename) {
    layers.clear();
    tilesets.clear();
    collisionObjects.clear();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir mapa JSON: " << filename << std::endl;
        return false;
    }

    json mapData;
    try {
        file >> mapData;
    } catch (std::exception& e) {
        std::cerr << "Erro ao parsear JSON: " << e.what() << std::endl;
        return false;
    }

    width = mapData["width"];
    height = mapData["height"];
    tileSize = mapData["tilewidth"];

    std::string mapDir;
    std::size_t separator = filename.find_last_of("/\\");
    if (separator != std::string::npos) {
        mapDir = filename.substr(0, separator + 1);
    }

    // Carregar os tilesets
    for (const auto& jTs : mapData["tilesets"]) {
        TilesetInfo ts;
        ts.firstGid = jTs["firstgid"];
        
        std::string imagePath = jTs["image"];
        std::string fullImagePath = mapDir + imagePath;
        
        if (!ts.texture.loadFromFile(fullImagePath)) {
            std::cerr << "Erro ao carregar textura do tileset: " << fullImagePath << std::endl;
        }
        
        ts.columns = jTs["columns"];
        ts.tileCount = jTs["tilecount"];
        tilesets.push_back(std::move(ts));
    }

    // Carregar as camadas
    for (const auto& jLayer : mapData["layers"]) {
        if (jLayer["type"] == "tilelayer") {
            TileLayer layer;
            layer.name = jLayer["name"];
            layer.visible = jLayer.value("visible", true);
            
            auto& data = jLayer["data"];
            for (int tileId : data) {
                layer.data.push_back(tileId);
            }
            
            buildVertices(layer);
            layers.push_back(std::move(layer));
        } else if (jLayer["type"] == "objectgroup") {
            std::string lowerName = jLayer["name"];
            for (auto& c : lowerName) c = tolower(c);
            
            // Verifica se a camada de objetos é destinada à colisão
            if (lowerName.find("colisao") != std::string::npos || lowerName.find("colisão") != std::string::npos) {
                if (jLayer.contains("objects")) {
                    for (const auto& obj : jLayer["objects"]) {
                        float ox = obj.value("x", 0.0f);
                        float oy = obj.value("y", 0.0f);
                        float ow = obj.value("width", 0.0f);
                        float oh = obj.value("height", 0.0f);
                        
                        // Ignora pontos e objetos sem dimensão física
                        if (ow > 0 && oh > 0) {
                            collisionObjects.emplace_back(ox, oy, ow, oh);
                        }
                    }
                }
            }
        }
    }

    return true;
}

int TileMap::findTilesetIndex(int tileId) const {
    for (int i = tilesets.size() - 1; i >= 0; --i) {
        if (tileId >= tilesets[i].firstGid) {
            return i;
        }
    }
    return -1;
}

void TileMap::buildVertices(TileLayer& layer) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            int tileNumber = layer.data[i + j * width];
            if (tileNumber == 0) continue; // Vazio

            int tsIndex = findTilesetIndex(tileNumber);
            if (tsIndex == -1) continue;

            auto& ts = tilesets[tsIndex];
            int localId = tileNumber - ts.firstGid;
            
            int tu = localId % ts.columns;
            int tv = localId / ts.columns;

            // Se ainda não temos um VertexArray para este tileset nesta camada, cria um
            if (layer.verticesPerTileset.find(tsIndex) == layer.verticesPerTileset.end()) {
                layer.verticesPerTileset[tsIndex].setPrimitiveType(sf::Quads);
            }

            auto& varray = layer.verticesPerTileset[tsIndex];
            
            sf::Vertex quad[4];
            quad[0].position = sf::Vector2f(i * tileSize, j * tileSize);
            quad[1].position = sf::Vector2f((i + 1) * tileSize, j * tileSize);
            quad[2].position = sf::Vector2f((i + 1) * tileSize, (j + 1) * tileSize);
            quad[3].position = sf::Vector2f(i * tileSize, (j + 1) * tileSize);

            quad[0].texCoords = sf::Vector2f(tu * tileSize, tv * tileSize);
            quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize, tv * tileSize);
            quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize, (tv + 1) * tileSize);
            quad[3].texCoords = sf::Vector2f(tu * tileSize, (tv + 1) * tileSize);

            for(int v=0; v<4; v++) varray.append(quad[v]);
        }
    }
}

bool TileMap::checkCollision(float worldX, float worldY) const {
    // 1. Checa contra os objetos livres de colisão (Sub-tile pixel perfect)
    for (const auto& rect : collisionObjects) {
        if (rect.contains(worldX, worldY)) {
            return true;
        }
    }

    // 2. Checa contra o grid de blocos normal
    const float GLOBAL_SCALE = 1.0f;
    int cellX = static_cast<int>(worldX / (tileSize * GLOBAL_SCALE));
    int cellY = static_cast<int>(worldY / (tileSize * GLOBAL_SCALE));

    if (cellX < 0 || cellX >= width || cellY < 0 || cellY >= height) return true; // Parede invisivel nas bordas

    for (const auto& layer : layers) {
        std::string lowerName = layer.name;
        for (auto& c : lowerName) c = tolower(c);
        
        if (lowerName.find("colisao") != std::string::npos || lowerName.find("colisão") != std::string::npos) {
            int index = cellX + cellY * width;
            if (layer.data[index] != 0) return true;
        }
    }
    return false;
}

void TileMap::drawLayer(sf::RenderTarget& target, sf::RenderStates states, const std::string& layerName) const {
    states.transform *= getTransform();
    for (const auto& layer : layers) {
        if (layer.visible && layer.name == layerName) {
            for (const auto& pair : layer.verticesPerTileset) {
                int tsIndex = pair.first;
                sf::RenderStates st = states;
                st.texture = &tilesets[tsIndex].texture;
                target.draw(pair.second, st);
            }
        }
    }
}

void TileMap::drawAll(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    for (const auto& layer : layers) {
        if (!layer.visible) continue;
        
        std::string lowerName = layer.name;
        for (auto& c : lowerName) c = tolower(c);
        if (lowerName.find("colisao") != std::string::npos || lowerName.find("colisão") != std::string::npos) continue;
        if (lowerName.find("frente") != std::string::npos || lowerName.find("topo") != std::string::npos || lowerName.find("acima") != std::string::npos) continue;

        for (const auto& pair : layer.verticesPerTileset) {
            int tsIndex = pair.first;
            sf::RenderStates st = states;
            st.texture = &tilesets[tsIndex].texture;
            target.draw(pair.second, st);
        }
    }
}

void TileMap::drawForeground(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    for (const auto& layer : layers) {
        if (!layer.visible) continue;
        
        std::string lowerName = layer.name;
        for (auto& c : lowerName) c = tolower(c);
        if (lowerName.find("frente") != std::string::npos || lowerName.find("topo") != std::string::npos || lowerName.find("acima") != std::string::npos) {
            for (const auto& pair : layer.verticesPerTileset) {
                int tsIndex = pair.first;
                sf::RenderStates st = states;
                st.texture = &tilesets[tsIndex].texture;
                target.draw(pair.second, st);
            }
        }
    }
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    drawAll(target, states);
}
