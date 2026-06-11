#include "entities.h"

#include <algorithm>

using namespace std;

bool mesmaPosicao(Posicao a, Posicao b) {
    return a.x == b.x && a.y == b.y;
}

void inicializarJogador(Jogador& jogador) {
    jogador.posicao = {1, 1};
    jogador.atributos = {2, 2, 2, 2};
    jogador.nivel = 1;
    jogador.xp = 0;
    jogador.xpProximoNivel = 80;
    jogador.chaves = 0;
    jogador.pocoes = 1;
    jogador.pontosDisponiveis = 0;
    jogador.movimentos = 0;
    jogador.itensColetados = 0;
    jogador.vivo = true;
    jogador.tempoAtaque = 0.0f;
    recalcularStatus(jogador);
    jogador.status.vida = jogador.status.vidaMaxima;
}

void recalcularStatus(Jogador& jogador) {
    int vidaAtual = jogador.status.vida;

    jogador.status.vidaMaxima = 35 + jogador.atributos.vitalidade * 8;
    jogador.status.ataque = 6 + jogador.atributos.forca * 3;
    jogador.status.armadura = jogador.atributos.defesa * 2;
    jogador.status.acerto = 70 + jogador.atributos.agilidade * 3;
    jogador.status.esquiva = 5 + jogador.atributos.agilidade * 4;
    jogador.status.velocidade = 1 + jogador.atributos.agilidade / 4;

    if (vidaAtual <= 0) {
        jogador.status.vida = jogador.status.vidaMaxima;
    } else {
        jogador.status.vida = min(vidaAtual, jogador.status.vidaMaxima);
    }
}

void distribuirPontoAtributo(Jogador& jogador, char escolha) {
    if (jogador.pontosDisponiveis <= 0) {
        return;
    }

    switch (escolha) {
    case '1':
        jogador.atributos.forca++;
        break;
    case '2':
        jogador.atributos.defesa++;
        break;
    case '3':
        jogador.atributos.agilidade++;
        break;
    case '4':
        jogador.atributos.vitalidade++;
        break;
    default:
        return;
    }

    jogador.pontosDisponiveis--;
    recalcularStatus(jogador);
}
