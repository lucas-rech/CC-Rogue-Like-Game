
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
using namespace sf;


int main() {
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow window(VideoMode(1920, 1080), "Rogue Like Game", Style::Fullscreen, settings);
    window.setFramerateLimit(60); // Fundamental para controlar a velocidade do jogo

    View view(FloatRect(0, 0, 480.f, 270.f));

    // --- CARREGAMENTO DE TEXTURAS ---
    Texture idleTexture;
    Texture walkTexture;

    if (!idleTexture.loadFromFile("assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/idle.png") ||
        !walkTexture.loadFromFile("assets/textures/characters/player/warrior/warrior_lvl1/Without_shadow/walk.png")) {
        cout << "Erro ao carregar as texturas!" << std::endl;
        return -1;
    }

    // Configurações iniciais do Sprite
    Sprite playerSprite;
    playerSprite.setTexture(idleTexture);
    playerSprite.setPosition(100.f, 100.f);

    Clock animationClock;
    int playerFrame = 0;
    int totalFrames = 12;
    int textureRow = 0;

    float animationSpeed = 1.f;
    float movementSpeed = 3.0f;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                window.close();
        }

        // --- ENTRADA DE MOVIMENTAÇÃO ---
        Vector2f movement(0.f, 0.f);
        bool isMoving = false;

        if (Keyboard::isKeyPressed(Keyboard::D)) {
            movement.x += movementSpeed;
            textureRow = 128; // Linha da direita
            isMoving = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            movement.x -= movementSpeed;
            textureRow = 64;  // Linha da esquerda
            isMoving = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::S)) {
            movement.y += movementSpeed;
            textureRow = 0;   // Linha de frente (baixo)
            isMoving = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::W)) {
            movement.y -= movementSpeed;
            textureRow = 192; // Linha de costas (cima)
            isMoving = true;
        }

        // Aplica o movimento na posição do sprite
        playerSprite.move(movement);

        if (isMoving) {
            playerSprite.setTexture(walkTexture);
            totalFrames = 6;          // O walk mantém 6 frames em todas as direções
            animationSpeed = 0.1f;
        } else {
            playerSprite.setTexture(idleTexture);
            animationSpeed = 0.12f;

            // Verifica a direção (linha da textura) para definir os frames do Idle
            if (textureRow == 192) {
                animationSpeed = 0.3f;
                totalFrames = 4;      // Idle para CIMA (Costas) tem apenas 4 frames
            } else {
                totalFrames = 12;     // As outras direções de Idle têm 16 frames
            }
        }

        // --- CONTROLE DA ANIMAÇÃO ---
        if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
            playerFrame++;

            if (playerFrame >= totalFrames) {
                playerFrame = 0;
            }
            animationClock.restart();
        }

        // Proteção crucial para quando o jogador solta a tecla:
        // Se ele estava andando (frame 5) e para olhando para cima (onde o máximo é 4),
        // reiniciamos imediatamente para o frame 0 para evitar ler texturas inexistentes.
        if (playerFrame >= totalFrames) {
            playerFrame = 0;
        }

        int rectLeft = playerFrame * 64;
        playerSprite.setTextureRect(IntRect(rectLeft, textureRow, 64, 64));

        window.setView(view);
        window.clear(Color::Black);
        window.draw(playerSprite);
        window.display();
    }

    return 0;
}