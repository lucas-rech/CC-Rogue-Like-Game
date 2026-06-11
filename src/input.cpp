#include "input.h"

char mapearTeclaSFML(sf::Keyboard::Key tecla) {
    switch (tecla) {
        case sf::Keyboard::W:
        case sf::Keyboard::Up:
            return 'w';
        case sf::Keyboard::S:
        case sf::Keyboard::Down:
            return 's';
        case sf::Keyboard::A:
        case sf::Keyboard::Left:
            return 'a';
        case sf::Keyboard::D:
        case sf::Keyboard::Right:
            return 'd';
        case sf::Keyboard::F:
            return 'f';
        case sf::Keyboard::P:
            return 'p';
        case sf::Keyboard::H:
            return 'h';
        case sf::Keyboard::Q:
        case sf::Keyboard::Escape:
            return 'q';
        case sf::Keyboard::Num1:
        case sf::Keyboard::Numpad1:
            return '1';
        case sf::Keyboard::Num2:
        case sf::Keyboard::Numpad2:
            return '2';
        case sf::Keyboard::Num3:
        case sf::Keyboard::Numpad3:
            return '3';
        case sf::Keyboard::Num4:
        case sf::Keyboard::Numpad4:
            return '4';
        default:
            return 0; // Nenhuma tecla mapeada
    }
}
