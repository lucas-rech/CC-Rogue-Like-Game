//
// Created by lucas on 11/06/2026.
//

#pragma once

/*
*
*   # representa as árvores densas (Paredes/Bloqueio).
*   . representa a grama livre (Chão).
*   ~ representa a água (Bloqueio).
*   + representa a saída/caverna para a próxima fase.
*   t representa trilhas
*   w parede de pedra
*   g transicao de grama
*
 **/

const int MAP_ROWS = 30;
const int MAP_COLS = 50;
const int TILE_SIZE = 64;

extern const char* rawMapLevel1[MAP_ROWS];

