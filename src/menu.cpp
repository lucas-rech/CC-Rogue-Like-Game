#include "menu.h"
#include <vector>
#include <string>
#include <utility>

namespace {
void desenharTexto(sf::RenderWindow& window, const sf::Font& font, const std::string& str, float x, float y, unsigned int size, sf::Color cor, bool bold = false) {
    sf::Text text;
    text.setFont(font);
    text.setString(sf::String::fromUtf8(str.begin(), str.end()));
    text.setCharacterSize(size);
    text.setFillColor(cor);
    text.setPosition(x, y);
    if (bold) {
        text.setStyle(sf::Text::Bold);
    }
    window.draw(text);
}

void desenharTeclaBadge(sf::RenderWindow& window, const sf::Font& font, const std::string& tecla, float x, float y) {
    sf::RectangleShape badge(sf::Vector2f(40.0f, 30.0f));
    badge.setFillColor(sf::Color(59, 130, 246)); // Blue-500
    badge.setOutlineThickness(1.0f);
    badge.setOutlineColor(sf::Color(147, 197, 253)); // Blue-300
    badge.setPosition(x, y);
    
    // Arredondamento simples simulado por posicionamento
    window.draw(badge);

    sf::Text text;
    text.setFont(font);
    text.setString(tecla);
    text.setCharacterSize(18);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::White);
    
    // Centralizar texto no badge
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        x + (40.0f - textBounds.width) / 2.0f - textBounds.left,
        y + (30.0f - textBounds.height) / 2.0f - textBounds.top
    );
    window.draw(text);
}
}

void renderizarMenuSFML(sf::RenderWindow& window, const sf::Font& font) {
    // Fundo Gradiente Simples
    sf::RectangleShape bg(sf::Vector2f(1200.f, 800.f));
    bg.setFillColor(sf::Color(15, 23, 42)); // Slate-900
    window.draw(bg);

    // Efeito decorativo
    sf::RectangleShape decor(sf::Vector2f(600.f, 10.f));
    decor.setFillColor(sf::Color(249, 115, 22)); // Orange-500
    decor.setPosition(300.f, 180.f);
    window.draw(decor);

    // Titulo
    desenharTexto(window, font, "CC ROGUELIKE DUNGEON", 300.f, 100.f, 44, sf::Color(251, 191, 36), true);
    desenharTexto(window, font, "Uma Aventura Grafica com SFML", 460.f, 195.f, 18, sf::Color(148, 163, 184));

    // Opcoes
    float startX = 400.f;
    float startY = 280.f;
    float spacingY = 60.f;

    desenharTeclaBadge(window, font, "1", startX, startY);
    desenharTexto(window, font, "Iniciar Novo Jogo", startX + 60.f, startY + 2.f, 22, sf::Color::White);

    desenharTeclaBadge(window, font, "2", startX, startY + spacingY);
    desenharTexto(window, font, "Como Jogar", startX + 60.f, startY + spacingY + 2.f, 22, sf::Color::White);

    desenharTeclaBadge(window, font, "3", startX, startY + 2 * spacingY);
    desenharTexto(window, font, "Guia de Itens", startX + 60.f, startY + 2 * spacingY + 2.f, 22, sf::Color::White);

    desenharTeclaBadge(window, font, "4", startX, startY + 3 * spacingY);
    desenharTexto(window, font, "Sistema de Pontuacao", startX + 60.f, startY + 3 * spacingY + 2.f, 22, sf::Color::White);

    desenharTeclaBadge(window, font, "Q", startX, startY + 4 * spacingY);
    desenharTexto(window, font, "Sair do Jogo", startX + 60.f, startY + 4 * spacingY + 2.f, 22, sf::Color(239, 68, 68));

    // Rodape
    desenharTexto(window, font, "Use os numeros do teclado para navegar", 440.f, 650.f, 16, sf::Color(100, 116, 139));
}

void renderizarComoJogarSFML(sf::RenderWindow& window, const sf::Font& font) {
    sf::RectangleShape bg(sf::Vector2f(1200.f, 800.f));
    bg.setFillColor(sf::Color(15, 23, 42)); // Slate-900
    window.draw(bg);

    desenharTexto(window, font, "COMO JOGAR", 470.f, 60.f, 36, sf::Color(251, 191, 36), true);

    float startY = 160.f;
    float spacingY = 45.f;
    float startX = 200.f;

    std::vector<std::string> instrucoes = {
        "WASD / Setas Direcionais : Mover o personagem",
        "F : Atacar inimigos em casas adjacentes",
        "Andar contra o Inimigo : Tambem realiza um ataque automatico",
        "P : Consumir uma Pocao de Cura (+22 de Vida)",
        "Teclas 1 a 4 : Distribuir pontos de atributo ao subir de nivel",
        "Q / ESC : Abandonar a masmorra e terminar o jogo",
        "Objetivo : Explore os 3 niveis, colete chaves para abrir portas (D),",
        "           encontre equipamentos e derrote o Chefe Final (👹) ao sul."
    };

    for (size_t i = 0; i < instrucoes.size(); i++) {
        desenharTexto(window, font, instrucoes[i], startX, startY + i * spacingY, 20, sf::Color::White);
    }

    desenharTexto(window, font, "Pressione qualquer outra tecla para voltar...", 420.f, 650.f, 18, sf::Color(148, 163, 184));
}

void renderizarExplicacaoItensSFML(sf::RenderWindow& window, const sf::Font& font) {
    sf::RectangleShape bg(sf::Vector2f(1200.f, 800.f));
    bg.setFillColor(sf::Color(15, 23, 42)); // Slate-900
    window.draw(bg);

    desenharTexto(window, font, "GUIA DE ITENS", 460.f, 60.f, 36, sf::Color(251, 191, 36), true);

    float startX = 250.f;
    float startY = 160.f;
    float spacingY = 75.f;

    // Pocao
    {
        sf::CircleShape icon(15.f);
        icon.setFillColor(sf::Color(34, 197, 94)); // Green
        icon.setPosition(startX, startY);
        window.draw(icon);
        desenharTexto(window, font, "Pocao de Cura (🧪)", startX + 50.f, startY - 2.f, 22, sf::Color(34, 197, 94), true);
        desenharTexto(window, font, "Armazena no inventario. Cura 22 de HP ao ser utilizada (P).", startX + 50.f, startY + 28.f, 18, sf::Color(203, 213, 225));
    }

    // Chave
    {
        sf::CircleShape icon(15.f);
        icon.setFillColor(sf::Color(234, 179, 8)); // Yellow
        icon.setPosition(startX, startY + spacingY);
        window.draw(icon);
        desenharTexto(window, font, "Chave (🔑)", startX + 50.f, startY + spacingY - 2.f, 22, sf::Color(234, 179, 8), true);
        desenharTexto(window, font, "Usada para abrir portas trancadas (🚪) pelo mapa.", startX + 50.f, startY + spacingY + 28.f, 18, sf::Color(203, 213, 225));
    }

    // Arma
    {
        sf::CircleShape icon(15.f);
        icon.setFillColor(sf::Color(239, 68, 68)); // Red
        icon.setPosition(startX, startY + 2 * spacingY);
        window.draw(icon);
        desenharTexto(window, font, "Arma / Machado (🪓)", startX + 50.f, startY + 2 * spacingY - 2.f, 22, sf::Color(239, 68, 68), true);
        desenharTexto(window, font, "Aumenta permanentemente a Forca e o Dano Fisico do jogador.", startX + 50.f, startY + 2 * spacingY + 28.f, 18, sf::Color(203, 213, 225));
    }

    // Escudo
    {
        sf::CircleShape icon(15.f);
        icon.setFillColor(sf::Color(59, 130, 246)); // Blue
        icon.setPosition(startX, startY + 3 * spacingY);
        window.draw(icon);
        desenharTexto(window, font, "Escudo / Armadura (💙)", startX + 50.f, startY + 3 * spacingY - 2.f, 22, sf::Color(59, 130, 246), true);
        desenharTexto(window, font, "Aumenta permanentemente a Defesa e a absorcao de dano.", startX + 50.f, startY + 3 * spacingY + 28.f, 18, sf::Color(203, 213, 225));
    }

    // Powerup
    {
        sf::CircleShape icon(15.f);
        icon.setFillColor(sf::Color(168, 85, 247)); // Purple
        icon.setPosition(startX, startY + 4 * spacingY);
        window.draw(icon);
        desenharTexto(window, font, "Cristal de Poder (✨)", startX + 50.f, startY + 4 * spacingY - 2.f, 22, sf::Color(168, 85, 247), true);
        desenharTexto(window, font, "Aumenta permanentemente a Agilidade e a Vitalidade.", startX + 50.f, startY + 4 * spacingY + 28.f, 18, sf::Color(203, 213, 225));
    }

    desenharTexto(window, font, "Pressione qualquer outra tecla para voltar...", 420.f, 680.f, 18, sf::Color(148, 163, 184));
}

void renderizarExplicacaoPontuacaoSFML(sf::RenderWindow& window, const sf::Font& font) {
    sf::RectangleShape bg(sf::Vector2f(1200.f, 800.f));
    bg.setFillColor(sf::Color(15, 23, 42)); // Slate-900
    window.draw(bg);

    desenharTexto(window, font, "SISTEMA DE PONTUACAO", 370.f, 60.f, 36, sf::Color(251, 191, 36), true);

    float startX = 350.f;
    float startY = 160.f;
    float spacingY = 40.f;

    std::vector<std::pair<std::string, sf::Color>> regras = {
        {"+100 pontos : Por inimigo comum derrotado", sf::Color(34, 197, 94)},
        {"+500 pontos : Por derrotar o Chefe Final (Boss)", sf::Color(34, 197, 94)},
        {"+50 pontos  : Por item coletado", sf::Color(34, 197, 94)},
        {"+200 pontos : Por andar concluido da masmorra", sf::Color(34, 197, 94)},
        {"-1 ponto    : Por movimento executado (tempo)", sf::Color(239, 68, 68)},
        {"-20 pontos  : Por pocao de cura utilizada", sf::Color(239, 68, 68)},
        {"-5 pontos   : Por ponto de dano recebido", sf::Color(239, 68, 68)},
        {"+10 pontos  : Multiplicador por HP restante no final", sf::Color(251, 191, 36)}
    };

    for (size_t i = 0; i < regras.size(); i++) {
        desenharTexto(window, font, regras[i].first, startX, startY + i * spacingY, 20, regras[i].second);
    }

    desenharTexto(window, font, "Tente terminar o jogo o mais rapido possivel,", 380.f, 520.f, 20, sf::Color(148, 163, 184));
    desenharTexto(window, font, "recebendo pouco dano e acumulando tesouros!", 380.f, 550.f, 20, sf::Color(148, 163, 184));

    desenharTexto(window, font, "Pressione qualquer outra tecla para voltar...", 420.f, 650.f, 18, sf::Color(148, 163, 184));
}
