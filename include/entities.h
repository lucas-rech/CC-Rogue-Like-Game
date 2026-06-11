#ifndef ENTITIES_H
#define ENTITIES_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

using namespace std;


const int TOTAL_NIVEIS = 3;
const int RAIO_VISAO = 5;
const int CODIGO_VAZIO = -1;
const int CODIGO_CHAO = 0;
const int CODIGO_PAREDE = 1;
const int CODIGO_PORTA = 2;
const int CODIGO_ARMADILHA = 3;
const int CODIGO_SAIDA = 4;
const int CODIGO_JOGADOR = 10;
const int CODIGO_INIMIGO_COMUM = 20;
const int CODIGO_INIMIGO_PERSEGUIDOR = 21;
const int CODIGO_INIMIGO_TANQUE = 22;
const int CODIGO_BOSS = 23;
const int CODIGO_ITEM_POCAO = 30;
const int CODIGO_ITEM_CHAVE = 31;
const int CODIGO_ITEM_ARMA = 32;
const int CODIGO_ITEM_ESCUDO = 33;
const int CODIGO_ITEM_POWERUP = 34;
const int CODIGO_NPC = 40;

struct Posicao {
    int x;
    int y;
};

struct Atributos {
    int forca;
    int defesa;
    int agilidade;
    int vitalidade;
};

struct Status {
    int vida;
    int vidaMaxima;
    int ataque;
    int armadura;
    int acerto;
    int esquiva;
    int velocidade;
};

struct Jogador {
    Posicao posicao;
    float x;
    float y;
    Status status;
    Atributos atributos;
    int nivel;
    int xp;
    int xpProximoNivel;
    int chaves;
    int pocoes;
    int pontosDisponiveis;
    int movimentos;
    int itensColetados;
    bool vivo;
    float tempoAtaque;
};

enum TipoInimigo {
    INIMIGO_ALEATORIO,
    INIMIGO_PERSEGUIDOR,
    INIMIGO_TANQUE,
    INIMIGO_BOSS
};

struct Inimigo {
    Posicao posicao;
    float x;
    float y;
    Status status;
    TipoInimigo tipo;
    int xpConcedido;
    bool vivo;
    float tempoAtaque;
};

enum TipoItem {
    ITEM_POCAO,
    ITEM_CHAVE,
    ITEM_ARMA,
    ITEM_ESCUDO,
    ITEM_POWERUP
};

struct Item {
    Posicao posicao;
    TipoItem tipo;
    int valor;
    bool coletado;
};

enum TipoTile {
    CHAO,
    PAREDE,
    PORTA,
    ARMADILHA,
    SAIDA,
    NPC_TILE
};

struct Tile {
    TipoTile tipo;
    bool visivel;
    bool explorado;
    bool bloqueiaMovimento;
};

struct NPC {
    Posicao posicao;
    string nome;
    string fala;
    bool interagiu;
};

struct Pontuacao {
    int inimigosDerrotados;
    int itensColetados;
    int niveisConcluidos;
    int movimentos;
    int danoRecebido;
    int pocoesUsadas;
    int bossDerrotado;
    int pontuacaoFinal;
};

struct Mapa {
    vector<vector<int> > matriz;
    vector<vector<Tile> > tiles;
    vector<Inimigo> inimigos;
    vector<Item> itens;
    vector<NPC> npcs;
    int largura;
    int altura;
    int nivelAtual;
};

enum class EstadoJogo {
    MENU,
    COMO_JOGAR,
    EXPLICACAO_ITENS,
    EXPLICACAO_PONTUACAO,
    JOGANDO,
    TELA_FINAL
};

struct TexturasJogo {
    sf::Texture jogador;
    sf::Texture inimigoComum;
    sf::Texture inimigoPerseguidor;
    sf::Texture inimigoTanque;
    sf::Texture boss;
    
    // Texturas de ataque
    sf::Texture jogadorAtaque;
    sf::Texture comumAtaque;
    sf::Texture perseguidorAtaque;
    sf::Texture tanqueAtaque;
    sf::Texture bossAtaque;
    
    bool jogadorCarregado = false;
    bool comumCarregado = false;
    bool perseguidorCarregado = false;
    bool tanqueCarregado = false;
    bool bossCarregado = false;
    
    bool jogadorAtaqueCarregado = false;
    bool comumAtaqueCarregado = false;
    bool perseguidorAtaqueCarregado = false;
    bool tanqueAtaqueCarregado = false;
    bool bossAtaqueCarregado = false;
};

struct Jogo {
    Jogador jogador;
    Mapa mapa;
    Pontuacao pontuacao;
    string mensagem;
    bool rodando;
    bool venceu;
    bool perdeu;
    EstadoJogo estado;
};

bool mesmaPosicao(Posicao a, Posicao b);
void inicializarJogador(Jogador& jogador);
void recalcularStatus(Jogador& jogador);
void distribuirPontoAtributo(Jogador& jogador, char escolha);

#endif
