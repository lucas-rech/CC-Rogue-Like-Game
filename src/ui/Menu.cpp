#include "Menu.hpp"

#include <cstdlib>
#include <vector>

Menu::Menu()
    : fontLoaded(false),
      selectedOption(0),
      selectedDifficulty(Difficulty::Easy),
      playerName(""),
      mainOptions{"Jogar", "Como jogar", "Itens do jogo", "Sistema de pontuacao", "Ranking", "Modo Auto-Play (IA)", "Sair"},
      difficultyOptions{"Facil", "Medio", "Dificil", "Voltar"} {
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
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            return true;
        }
    }

    fontLoaded = false;
    return false;
}

GameScreen Menu::handleEvent(const sf::Event& event, GameScreen screen) {
    if (screen == GameScreen::PlayerName) {
        return handleNameInput(event);
    }

    if (event.type != sf::Event::KeyPressed) {
        return screen;
    }

    if (screen == GameScreen::HowToPlay || screen == GameScreen::ItemsInfo || screen == GameScreen::ScoreInfo) {
        if (event.key.code == sf::Keyboard::Escape) {
            selectedOption = 0;
            return GameScreen::Menu;
        }
        return screen;
    }

    if (event.key.code == sf::Keyboard::Escape) {
        selectedOption = 0;
        return GameScreen::Menu;
    }

    const int optionCount = screen == GameScreen::DifficultySelect
        ? static_cast<int>(difficultyOptions.size())
        : static_cast<int>(mainOptions.size());

    if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
        moveSelection(-1, optionCount);
    } else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
        moveSelection(1, optionCount);
    } else if (event.key.code == sf::Keyboard::Enter) {
        if (screen == GameScreen::DifficultySelect) {
            return selectDifficultyOption();
        }
        return selectMainOption();
    }

    return screen;
}

Difficulty Menu::getSelectedDifficulty() const {
    return selectedDifficulty;
}

const std::string& Menu::getPlayerName() const {
    return playerName;
}

void Menu::draw(sf::RenderWindow& window, GameScreen screen) {
    if (!fontLoaded) {
        return;
    }

    if (screen == GameScreen::Menu) {
        drawMainMenu(window);
        return;
    }

    if (screen == GameScreen::PlayerName) {
        drawNameInput(window);
        return;
    }

    if (screen == GameScreen::DifficultySelect) {
        drawDifficultyMenu(window);
        return;
    }

    if (screen == GameScreen::HowToPlay) {
        drawInfoScreen(window, "Como jogar", {
            "Movimento: use W, A, S e D para andar pelo mapa.",
            "Objetivo: colete as 3 Chaves para abrir o Portao do Castelo e derrote o Conde Vampiro (Boss)!",
            "Ataque Fisico: clique com o botao ESQUERDO do mouse para usar a espada.",
            "Ataque Magico: segure o botao DIREITO para mirar e solte para atirar (custa 20 Mana).",
            "Atributos: suba de nivel para ganhar pontos. Aperte I para distribuir os pontos.",
            "Interacao: aproxime-se do castelo com as 3 chaves para abrir a barricada magica.",
            "",
            "Pressione Esc para voltar ao menu."
        });
    } else if (screen == GameScreen::ItemsInfo) {
        drawInfoScreen(window, "Itens do jogo", {
            "Pocao de vida: recupera parte da vida do jogador.",
            "Pocao de mana: recupera parte da mana do jogador.",
            "Chave: permite abrir a passagem final do castelo.",
            "Escudo: aumenta a armadura do jogador.",
            "Livro de Magia: libera o ataque magico.",
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

void Menu::moveSelection(int direction, int optionCount) {
    selectedOption += direction;

    if (selectedOption < 0) {
        selectedOption = optionCount - 1;
    } else if (selectedOption >= optionCount) {
        selectedOption = 0;
    }
}

GameScreen Menu::selectMainOption() {
    switch (selectedOption) {
        case 0:
            selectedOption = 0;
            playerName.clear();
            return GameScreen::PlayerName;
        case 1: return GameScreen::HowToPlay;
        case 2: return GameScreen::ItemsInfo;
        case 3: return GameScreen::ScoreInfo;
        case 4: return GameScreen::Ranking;
        case 5:
            // AutoPlay start
            selectedOption = 0;
            playerName = "AutoPlay-AI";
            selectedDifficulty = Difficulty::Easy;
            return GameScreen::Playing; // The main loop will need to catch this and enable AutoPlay
        case 6: return GameScreen::Exit;
        default: return GameScreen::Menu;
    }
}

GameScreen Menu::selectDifficultyOption() {
    switch (selectedOption) {
        case 0:
            selectedDifficulty = Difficulty::Easy;
            return GameScreen::Playing;
        case 1:
            selectedDifficulty = Difficulty::Medium;
            return GameScreen::Playing;
        case 2:
            selectedDifficulty = Difficulty::Hard;
            return GameScreen::Playing;
        case 3:
            selectedOption = 0;
            return GameScreen::PlayerName;
        default:
            return GameScreen::DifficultySelect;
    }
}

GameScreen Menu::handleNameInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            playerName.clear();
            selectedOption = 0;
            return GameScreen::Menu;
        }

        if (event.key.code == sf::Keyboard::Enter) {
            if (playerName.empty()) {
                playerName = "Jogador";
            }
            selectedOption = 0;
            return GameScreen::DifficultySelect;
        }

        if (event.key.code == sf::Keyboard::BackSpace && !playerName.empty()) {
            playerName.pop_back();
        }
    } else if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode >= 32 && event.text.unicode <= 126 && playerName.size() < 20) {
            playerName.push_back(static_cast<char>(event.text.unicode));
        }
    }

    return GameScreen::PlayerName;
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

    for (int i = 0; i < static_cast<int>(mainOptions.size()); ++i) {
        sf::Color color = i == selectedOption ? sf::Color(255, 220, 120) : sf::Color::White;
        std::string prefix = i == selectedOption ? "> " : "  ";
        window.draw(makeText(prefix + mainOptions[i], 30, color, 150.f, 230.f + i * 48.f));
    }
}

void Menu::drawNameInput(sf::RenderWindow& window) {
    sf::RectangleShape background(sf::Vector2f(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    ));
    background.setFillColor(sf::Color(18, 22, 30));
    window.draw(background);

    window.draw(makeText("Digite seu nome:", 40, sf::Color(255, 220, 120), 120.f, 120.f));

    sf::RectangleShape inputBox(sf::Vector2f(520.f, 56.f));
    inputBox.setPosition(120.f, 200.f);
    inputBox.setFillColor(sf::Color(30, 36, 48));
    inputBox.setOutlineThickness(2.f);
    inputBox.setOutlineColor(sf::Color(190, 195, 205));
    window.draw(inputBox);

    const std::string displayName = playerName.empty() ? "Jogador" : playerName;
    window.draw(makeText(displayName, 30, sf::Color::White, 140.f, 212.f));
    window.draw(makeText("Enter confirma. Backspace apaga. Esc volta ao menu.", 20, sf::Color(190, 195, 205), 120.f, 290.f));
}

void Menu::drawDifficultyMenu(sf::RenderWindow& window) {
    sf::RectangleShape background(sf::Vector2f(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    ));
    background.setFillColor(sf::Color(18, 22, 30));
    window.draw(background);

    window.draw(makeText("Selecione a dificuldade", 40, sf::Color(255, 220, 120), 120.f, 100.f));
    window.draw(makeText("Jogador: " + playerName, 22, sf::Color(190, 195, 205), 120.f, 155.f));

    for (int i = 0; i < static_cast<int>(difficultyOptions.size()); ++i) {
        sf::Color color = i == selectedOption ? sf::Color(255, 220, 120) : sf::Color::White;
        std::string prefix = i == selectedOption ? "> " : "  ";
        window.draw(makeText(prefix + difficultyOptions[i], 30, color, 150.f, 230.f + i * 48.f));
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