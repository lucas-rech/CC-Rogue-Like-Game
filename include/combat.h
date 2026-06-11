#ifndef COMBAT_H
#define COMBAT_H

#include "entities.h"

void atacarInimigo(Jogo& jogo, Inimigo& inimigo);
void inimigoAtacaJogador(Jogo& jogo, Inimigo& inimigo);
int calcularDano(const Status& atacante, const Status& defensor);
bool acertouAtaque(const Status& atacante, const Status& defensor);
void derrotarInimigo(Jogo& jogo, Inimigo& inimigo);
void adicionarXP(Jogo& jogo, int xp);
void verificarSubidaDeNivel(Jogo& jogo);

#endif
