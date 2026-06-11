#include "combat.h"

#include <algorithm>
#include <cstdlib>

using namespace std;

int calcularDano(const Status& atacante, const Status& defensor) {
    int variacao = rand() % 5;
    return max(1, atacante.ataque + variacao - defensor.armadura);
}

bool acertouAtaque(const Status& atacante, const Status& defensor) {
    int chance = atacante.acerto - defensor.esquiva;
    chance = max(15, min(95, chance));
    return rand() % 100 < chance;
}

void atacarInimigo(Jogo& jogo, Inimigo& inimigo) {
    jogo.jogador.tempoAtaque = 0.4f; // Trigger attack animation
    if (!acertouAtaque(jogo.jogador.status, inimigo.status)) {
        jogo.mensagem = "Voce errou o ataque.";
        return;
    }

    int dano = calcularDano(jogo.jogador.status, inimigo.status);
    inimigo.status.vida -= dano;
    jogo.mensagem = "Voce causou " + to_string(dano) + " de dano.";

    if (inimigo.status.vida <= 0) {
        derrotarInimigo(jogo, inimigo);
    }
}

void inimigoAtacaJogador(Jogo& jogo, Inimigo& inimigo) {
    if (!inimigo.vivo) {
        return;
    }
    inimigo.tempoAtaque = 0.4f; // Trigger attack animation

    if (!acertouAtaque(inimigo.status, jogo.jogador.status)) {
        jogo.mensagem = "Um inimigo atacou, mas voce desviou.";
        return;
    }

    int dano = calcularDano(inimigo.status, jogo.jogador.status);
    jogo.jogador.status.vida -= dano;
    jogo.pontuacao.danoRecebido += dano;
    jogo.mensagem = "Um inimigo causou " + to_string(dano) + " de dano.";

    if (jogo.jogador.status.vida <= 0) {
        jogo.jogador.vivo = false;
        jogo.perdeu = true;
        jogo.rodando = false;
    }
}

void derrotarInimigo(Jogo& jogo, Inimigo& inimigo) {
    inimigo.vivo = false;
    jogo.pontuacao.inimigosDerrotados++;
    if (inimigo.tipo == INIMIGO_BOSS) {
        jogo.pontuacao.bossDerrotado = 1;
        jogo.venceu = true;
        jogo.rodando = false;
        jogo.mensagem = "O chefe final foi derrotado!";
    } else {
        jogo.mensagem = "Inimigo derrotado!";
    }
    adicionarXP(jogo, inimigo.xpConcedido);
}

void adicionarXP(Jogo& jogo, int xp) {
    jogo.jogador.xp += xp;
    verificarSubidaDeNivel(jogo);
}

void verificarSubidaDeNivel(Jogo& jogo) {
    while (jogo.jogador.xp >= jogo.jogador.xpProximoNivel) {
        jogo.jogador.xp -= jogo.jogador.xpProximoNivel;
        jogo.jogador.nivel++;
        jogo.jogador.pontosDisponiveis += 2;
        jogo.jogador.xpProximoNivel += 60;
        jogo.jogador.status.vida = jogo.jogador.status.vidaMaxima;
        jogo.mensagem = "Voce subiu de nivel! Use 1-4 para distribuir atributos.";
    }
}
