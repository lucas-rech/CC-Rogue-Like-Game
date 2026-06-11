#ifndef RENDER_H
#define RENDER_H

#include "entities.h"
#include <SFML/Graphics.hpp>

void configurarConsole();
void renderizarJogoSFML(sf::RenderWindow& window, const Jogo& jogo, const sf::Font& font, const TexturasJogo& texturas);
void mostrarTelaFinalSFML(sf::RenderWindow& window, const Jogo& jogo, const sf::Font& font);

#endif
