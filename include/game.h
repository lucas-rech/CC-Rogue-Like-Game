#ifndef GAME_H
#define GAME_H

#include "entities.h"

void inicializarJogo(Jogo& jogo);
void processarEntrada(Jogo& jogo, char tecla);
bool tentarMoverJogador(Jogo& jogo, int deslocamentoX, int deslocamentoY);
bool atacarInimigoAdjacente(Jogo& jogo);
void atualizarInimigos(Jogo& jogo, float dt);
void atualizarJogoRealTime(Jogo& jogo, float dt, float dx, float dy);
void coletarItem(Jogo& jogo, Item& item);
void usarPocao(Jogo& jogo);
bool tentarAbrirPorta(Jogo& jogo, Posicao posicao);
void ativarArmadilha(Jogo& jogo, Posicao posicao);
void aplicarEfeitoDoItem(Jogador& jogador, TipoItem tipo, int valor);
void interagirComNPC(Jogo& jogo, NPC& NPC);
void atualizarPontuacao(Jogo& jogo);
int calcularPontuacaoFinal(const Jogo& jogo);
void verificarCondicoesFim(Jogo& jogo);

#endif
