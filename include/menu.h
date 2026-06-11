#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>

void renderizarMenuSFML(sf::RenderWindow& window, const sf::Font& font);
void renderizarComoJogarSFML(sf::RenderWindow& window, const sf::Font& font);
void renderizarExplicacaoItensSFML(sf::RenderWindow& window, const sf::Font& font);
void renderizarExplicacaoPontuacaoSFML(sf::RenderWindow& window, const sf::Font& font);

#endif
