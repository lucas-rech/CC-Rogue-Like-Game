#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>

int main() {
    // Semente para gerar posições aleatórias da comida
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // 1. Inicializa a janela do jogo (Largura, Altura, Título)
    sf::RenderWindow window(sf::VideoMode(800, 600), "Meu Primeiro Jogo SFML - Cobrinha");
    window.setFramerateLimit(60); // Limita o jogo a 60 FPS

    // 2. Criando o Jogador (Cobra)
    sf::CircleShape player(20);
    player.setFillColor(sf::Color::Green);
    player.setPosition(400.0f, 300.0f); // Posição inicial no centro
    float speed = 5.0f;

    // 3. Criando a Comida
    sf::RectangleShape food(sf::Vector2f(30.0f, 30.0f));
    food.setFillColor(sf::Color::Red);
    food.setPosition(200.0f, 150.0f); // Posição inicial da comida

    // --- GAME LOOP ---
    while (window.isOpen()) {

        // --- 1. PROCESSAR INPUTS / EVENTOS ---
        sf::Event event;
        while (window.pollEvent(event)) {
            // Se o usuário clicar no 'X' para fechar a janela
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Movimentação em tempo real (segurando as teclas)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            player.move(-speed, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            player.move(speed, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            player.move(0.0f, -speed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            player.move(0.0f, speed);
        }

        // --- 2. ATUALIZAR LÓGICA DO JOGO (Colisões) ---
        // Pega as caixas de colisão (Bounding Boxes) do jogador e da comida
        sf::FloatRect playerBounds = player.getGlobalBounds();
        sf::FloatRect foodBounds = food.getGlobalBounds();

        // Se o quadrado verde encostar no quadrado vermelho...
        if (playerBounds.intersects(foodBounds)) {
            // Sorteia uma nova posição para a comida dentro dos limites da tela
            float newX = static_cast<float>(std::rand() % 750);
            float newY = static_cast<float>(std::rand() % 550);
            food.setPosition(newX, newY);

            std::cout << "Comida coletada! Nova posicao: (" << newX << ", " << newY << ")\n";
        }

        // --- 3. RENDERIZAR (DESENHAR NA TELA) ---
        window.clear(sf::Color::Black); // Limpa a tela com a cor preta

        window.draw(player); // Desenha o jogador
        window.draw(food);   // Desenha a comida

        window.display(); // Exibe tudo o que foi desenhado na tela
    }

    return 0;
}