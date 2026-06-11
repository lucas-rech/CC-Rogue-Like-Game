#include <SFML/Graphics.hpp>
#include <iostream>

#include "world/Level.hpp"
#include "graphics/TileMap.hpp"

using namespace sf;

int main() {
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow window(VideoMode(1920, 1080), "Rogue Like Game", Style::Fullscreen, settings);
    window.setFramerateLimit(60);

    // Zoom na tela
    View view(FloatRect(0, 0, 480.f, 270.f));

    // --- 1. CARREGAMENTO DE TEXTURAS ---
    Texture idleTexture;
    Texture walkTexture;

    if (!idleTexture.loadFromFile("assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/idle.png") ||
        !walkTexture.loadFromFile("assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/walk.png")) {
        std::cout << "Erro ao carregar as texturas do jogador!" << std::endl;
        return -1;
    }

    // --- 2. CONFIGURAÇÕES DO JOGADOR ---
    Sprite playerSprite;
    playerSprite.setTexture(idleTexture);

    Clock animationClock;
    int playerFrame = 0;
    int totalFrames = 12;
    int textureRow = 0;
    float animationSpeed = 0.08f;
    float movementSpeed = 1.5f; // Reduzido pois o mapa não está mais esticado 4x

    // --- 3. CONFIGURAÇÕES DO MAPA ---
    GameState gameState = {0};
    Sprite tileSprite;

    // Carrega a matriz da Fase 1 e posiciona o jogador automaticamente
    loadLevel(gameState, 1, playerSprite);

    // --- LOOP PRINCIPAL ---
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) window.close();
        }

        // --- ENTRADA DE MOVIMENTAÇÃO ---
        Vector2f movement(0.f, 0.f);
        bool isMoving = false;

        if (Keyboard::isKeyPressed(Keyboard::D)) { movement.x += movementSpeed; textureRow = 128; isMoving = true; }
        if (Keyboard::isKeyPressed(Keyboard::A)) { movement.x -= movementSpeed; textureRow = 64;  isMoving = true; }
        if (Keyboard::isKeyPressed(Keyboard::S)) { movement.y += movementSpeed; textureRow = 0;   isMoving = true; }
        if (Keyboard::isKeyPressed(Keyboard::W)) { movement.y -= movementSpeed; textureRow = 192; isMoving = true; }

        // --- SISTEMA DE COLISÃO DO MAPA ---
        if (isMoving) {
            // Cria uma "hitbox" menor, focada nos pés do jogador
            FloatRect nextHitbox = playerSprite.getGlobalBounds();
            // Assumindo que o personagem ocupa o centro do sprite 64x64
            nextHitbox.width = 12.f; 
            nextHitbox.height = 8.f;
            nextHitbox.left += movement.x + 26.f; // Centralizado no bloco de 64
            nextHitbox.top += movement.y + 40.f;  // Pés do personagem

            // Se o caminho estiver livre, aplica o movimento
            if (!checkCollision(nextHitbox, gameState)) {
                playerSprite.move(movement);
            }
        }

        // --- CONTROLE DINÂMICO DOS FRAMES POR ESTADO E DIREÇÃO ---
        if (isMoving) {
            playerSprite.setTexture(walkTexture);
            totalFrames = 6;
            animationSpeed = 0.1f;
        } else {
            playerSprite.setTexture(idleTexture);
            animationSpeed = 0.12f;
            if (textureRow == 192) {
                totalFrames = 4;
            } else {
                totalFrames = 12;
            }
        }

        // --- CONTROLE DA ANIMAÇÃO DO SPRITE ---
        if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
            playerFrame++;
            if (playerFrame >= totalFrames) playerFrame = 0;
            animationClock.restart();
        }
        if (playerFrame >= totalFrames) playerFrame = 0; // Proteção extra

        int rectLeft = playerFrame * 64;
        playerSprite.setTextureRect(IntRect(rectLeft, textureRow, 64, 64));

        // --- ATUALIZAÇÃO DA CÂMERA ---
        view.setCenter(playerSprite.getPosition());

        // --- RENDERIZAÇÃO ---
        window.clear(Color(163, 179, 21)); // Cor #a3b315

        // Aplica a câmera
        window.setView(view);

        // 1. Desenha as camadas de Chão
        // Não escalamos o mapa aqui, pois a View já aplica um zoom de 4x (1920x1080 / 480x270)
        gameState.map.drawLayer(window, RenderStates::Default, "Chao Base");
        gameState.map.drawLayer(window, RenderStates::Default, "Chao");
        gameState.map.drawLayer(window, RenderStates::Default, "Chao I");

        // 2. Desenha o Jogador
        window.draw(playerSprite);

        // 3. Desenha as camadas de Objetos (Por cima do jogador temporariamente para teste)
        gameState.map.drawLayer(window, RenderStates::Default, "Montanhas");
        gameState.map.drawLayer(window, RenderStates::Default, "Montanhas II");
        gameState.map.drawLayer(window, RenderStates::Default, "Arvores I");
        gameState.map.drawLayer(window, RenderStates::Default, "Arvores II");

        window.display();
    }

    return 0;
}