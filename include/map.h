#ifndef MAP_H
#define MAP_H

#include "entities.h"

void carregarNivel(Jogo& jogo, int nivel);
void gerarMapa(Mapa& mapa, int nivel);
void posicionarJogador(Jogo& jogo);
void criarInimigos(Mapa& mapa, int nivel);
void criarItens(Mapa& mapa, int nivel);
void criarNPCs(Mapa& mapa, int nivel);

bool posicaoValida(const Mapa& mapa, Posicao posicao);
bool existeParedeOuBloqueio(const Mapa& mapa, Posicao posicao);
bool dentroDoRaioDeVisao(Posicao origem, Posicao alvo, int raio);
void atualizarCampoDeVisao(Jogo& jogo);

Inimigo* buscarInimigoNaPosicao(Mapa& mapa, Posicao posicao);
Item* buscarItemNaPosicao(Mapa& mapa, Posicao posicao);
NPC* buscarNPCNaPosicao(Mapa& mapa, Posicao posicao);

void alterarMapaPorNPC(Jogo& jogo, NPC& NPC);
void alterarMapaPorEvento(Jogo& jogo, Posicao posicao);

#endif
