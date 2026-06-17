#include "Menu.hpp"

#include <cstdlib>
#include <vector>
#include <filesystem>

Menu::Menu()
    : fontLoaded(false),
      selectedOption(0),
      options{"Iniciar jogo", "Como jogar", "Itens do jogo", "Sistema de pontuacao", "Sair"} {
}

bool Menu::loadFont() {
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "assets/fonts/DejaVuSans.ttf",
        "assets/fonts/OpenSans-Regular.ttf",
        "assets/fonts/Roboto-Regular.ttf"
    };

    if (const char* windowsDir = std::getenv("WINDIR")) {
        fontPaths.emplace_back(std::string(windowsDir) + "/Fonts/arial.ttf");
    }

    for (const auto& path : fontPaths) {
        if (std::filesystem::exists(path) && font.loadFromFile(path)) {
            fontLoaded = true;
            return true;
        }
    }

    fontLoaded = false;
    return false;
}

GameScreen Menu::handleEvent(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed) {
        return GameScreen::Menu;
    }

    if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
        moveSelection(-1);
    } else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
        moveSelection(1);
    } else if (event.key.code == sf::Keyboard::Enter) {
        return selectCurrentOption();
    }

    return GameScreen::Menu;
}

void Menu::draw(sf::RenderWindow& window, GameScreen screen) {
    if (!fontLoaded) {
        return;
    }

    if (screen == GameScreen::Menu) {
        drawMainMenu(window);
        return;
    }

    if (screen == GameScreen::HowToPlay) {
        drawInfoScreen(window, "Como jogar", {
            "Movimento: use W, A, S e D para andar pelo mapa.",
            "Objetivo: explore a masmorra, colete 3 Chaves e derrote o Conde Vampiro (Boss)!",
            "Ataque Físico: clique com o botao ESQUERDO do mouse para usar a espada.",
            "Ataque Mágico: segure o botao DIREITO para mirar e solte para atirar (Custa 20 Mana).",
            "Atributos: suba de nível para ganhar pontos. Aperte 'I' para distribuir os pontos.",
            "Interação: aproxime-se do castelo com as 3 chaves para abrir a barricada mágica.",
            "",
            "Pressione Esc para voltar ao menu."
        });
    } else if (screen == GameScreen::ItemsInfo) {
        drawInfoScreen(window, "Itens do jogo", {
            "Pocao de vida: recupera parte da vida do jogador.",
            "Chave: abre portas ou passagens bloqueadas.",
            "Arma: aumenta o dano causado aos inimigos.",
            "Escudo: reduz o dano recebido.",
            "Power-up: melhora temporariamente algum atributo do personagem.",
            "",
            "Pressione Esc para voltar ao menu."
        });
    } else if (screen == GameScreen::ScoreInfo) {
        drawInfoScreen(window, "Sistema de pontuacao", {
            "Inimigos derrotados somam pontos.",
            "Itens coletados somam pontos.",
            "Avancar de nivel soma pontos.",
            "Movimentos podem reduzir pontos.",
            "Perder vida pode reduzir pontos.",
            "A pontuacao aparece no HUD durante o jogo.",
            "",
            "Pressione Esc para voltar ao menu."
        });
    }
}

void Menu::moveSelection(int direction) {
    selectedOption += direction;

    if (selectedOption < 0) {
        selectedOption = static_cast<int>(options.size()) - 1;
    } else if (selectedOption >= static_cast<int>(options.size())) {
        selectedOption = 0;
    }
}

GameScreen Menu::selectCurrentOption() const {
    switch (selectedOption) {
        case 0: return GameScreen::Playing;
        case 1: return GameScreen::HowToPlay;
        case 2: return GameScreen::ItemsInfo;
        case 3: return GameScreen::ScoreInfo;
        case 4: return GameScreen::Exit;
        default: return GameScreen::Menu;
    }
}

void Menu::drawMainMenu(sf::RenderWindow& window) {
    sf::RectangleShape background(sf::Vector2f(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    ));
    background.setFillColor(sf::Color(18, 22, 30));
    window.draw(background);

    window.draw(makeText("O Reino Perdido", 44, sf::Color::White, 120.f, 90.f));
    window.draw(makeText("Use W/S ou setas para navegar. Enter seleciona.", 20, sf::Color(190, 195, 205), 120.f, 150.f));

    for (int i = 0; i < static_cast<int>(options.size()); ++i) {
        sf::Color color = i == selectedOption ? sf::Color(255, 220, 120) : sf::Color::White;
        std::string prefix = i == selectedOption ? "> " : "  ";
        window.draw(makeText(prefix + options[i], 30, color, 150.f, 230.f + i * 48.f));
    }
}

void Menu::drawInfoScreen(sf::RenderWindow& window, const std::string& title, const std::vector<std::string>& lines) {
    sf::RectangleShape background(sf::Vector2f(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    ));
    background.setFillColor(sf::Color(18, 22, 30));
    window.draw(background);

    window.draw(makeText(title, 40, sf::Color(255, 220, 120), 100.f, 80.f));

    float y = 155.f;
    for (const auto& line : lines) {
        window.draw(makeText(line, 22, sf::Color::White, 110.f, y));
        y += 34.f;
    }
}

sf::Text Menu::makeText(const std::string& content, unsigned int size, sf::Color color, float x, float y) const {
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}
