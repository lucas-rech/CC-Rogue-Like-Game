#include "map.h"

#include <cmath>

namespace {
Tile criarTile(TipoTile tipo) {
    Tile tile;
    tile.tipo = tipo;
    tile.visivel = false;
    tile.explorado = false;
    tile.bloqueiaMovimento = tipo == PAREDE || tipo == PORTA;
    return tile;
}

bool codigoBloqueiaMovimento(int codigo) {
    return codigo == CODIGO_VAZIO || codigo == CODIGO_PAREDE || codigo == CODIGO_PORTA;
}

TipoTile tipoPorCodigo(int codigo) {
    if (codigo == CODIGO_PAREDE) return PAREDE;
    if (codigo == CODIGO_PORTA) return PORTA;
    if (codigo == CODIGO_ARMADILHA) return ARMADILHA;
    if (codigo == CODIGO_SAIDA) return SAIDA;
    return CHAO;
}

void aplicarLayout(Mapa& mapa, const vector<vector<int> >& desenho) {
    mapa.altura = static_cast<int>(desenho.size());
    mapa.largura = static_cast<int>(desenho[0].size());
    mapa.matriz = desenho;
    mapa.tiles.assign(mapa.altura, vector<Tile>(mapa.largura, criarTile(CHAO)));

    for (int y = 0; y < mapa.altura; y++) {
        for (int x = 0; x < mapa.largura; x++) {
            mapa.tiles[y][x] = criarTile(tipoPorCodigo(mapa.matriz[y][x]));
            mapa.tiles[y][x].bloqueiaMovimento = codigoBloqueiaMovimento(mapa.matriz[y][x]);
        }
    }
}

Inimigo criarInimigo(int x, int y, TipoInimigo tipo, int nivel) {
    Inimigo inimigo;
    inimigo.posicao = {x, y};
    inimigo.x = x * 80.0f + 40.0f;
    inimigo.y = y * 80.0f + 40.0f;
    inimigo.tipo = tipo;
    inimigo.vivo = true;
    inimigo.tempoAtaque = 0.0f;
    inimigo.xpConcedido = 35 + nivel * 15;

    if (tipo == INIMIGO_ALEATORIO) {
        inimigo.status = {18 + nivel * 3, 18 + nivel * 3, 6 + nivel, 1 + nivel, 62, 6, 1};
    } else if (tipo == INIMIGO_PERSEGUIDOR) {
        inimigo.status = {22 + nivel * 4, 22 + nivel * 4, 8 + nivel, 2 + nivel, 68, 10, 1};
    } else if (tipo == INIMIGO_TANQUE) {
        inimigo.status = {34 + nivel * 5, 34 + nivel * 5, 7 + nivel, 5 + nivel, 58, 3, 1};
    } else {
        inimigo.status = {90, 90, 16, 7, 78, 12, 1};
        inimigo.xpConcedido = 180;
    }

    return inimigo;
}

Item criarItem(int x, int y, TipoItem tipo, int valor) {
    Item item;
    item.posicao = {x, y};
    item.tipo = tipo;
    item.valor = valor;
    item.coletado = false;
    return item;
}

void colocarElemento(vector<vector<int> >& desenho, int x, int y, int codigo) {
    desenho[y][x] = codigo;
}

void desenharSala(vector<vector<int> >& desenho, int colunaInicial, int linhaInicial, int colunaFinal, int linhaFinal) {
    for (int y = linhaInicial; y <= linhaFinal; y++) {
        for (int x = colunaInicial; x <= colunaFinal; x++) {
            bool borda = y == linhaInicial || y == linhaFinal || x == colunaInicial || x == colunaFinal;
            desenho[y][x] = borda ? CODIGO_PAREDE : CODIGO_CHAO;
        }
    }
}

void desenharCorredorHorizontal(vector<vector<int> >& desenho, int colunaInicial, int colunaFinal, int y) {
    if (colunaInicial > colunaFinal) {
        int valorTemporario = colunaInicial;
        colunaInicial = colunaFinal;
        colunaFinal = valorTemporario;
    }

    for (int x = colunaInicial; x <= colunaFinal; x++) {
        desenho[y][x] = CODIGO_CHAO;
        if (y - 1 >= 0 && desenho[y - 1][x] != CODIGO_CHAO) desenho[y - 1][x] = CODIGO_PAREDE;
        if (y + 1 < static_cast<int>(desenho.size()) && desenho[y + 1][x] != CODIGO_CHAO) desenho[y + 1][x] = CODIGO_PAREDE;
    }
}

void desenharCorredorVertical(vector<vector<int> >& desenho, int x, int linhaInicial, int linhaFinal) {
    if (linhaInicial > linhaFinal) {
        int valorTemporario = linhaInicial;
        linhaInicial = linhaFinal;
        linhaFinal = valorTemporario;
    }

    for (int y = linhaInicial; y <= linhaFinal; y++) {
        desenho[y][x] = CODIGO_CHAO;
        if (x - 1 >= 0 && desenho[y][x - 1] != CODIGO_CHAO) desenho[y][x - 1] = CODIGO_PAREDE;
        if (x + 1 < static_cast<int>(desenho[y].size()) && desenho[y][x + 1] != CODIGO_CHAO) desenho[y][x + 1] = CODIGO_PAREDE;
    }
}

vector<vector<int> > criarDesenhoNivel(int nivel) {
    vector<vector<int> > desenho(50, vector<int>(100, CODIGO_VAZIO));

    // 3x3 layout of rooms
    // Row 1 rooms
    desenharSala(desenho, 2, 2, 18, 10);      // Sala 1 (Top-Left)
    desenharSala(desenho, 40, 2, 58, 10);     // Sala 2 (Top-Middle)
    desenharSala(desenho, 80, 2, 96, 10);     // Sala 3 (Top-Right)

    // Row 2 rooms
    desenharSala(desenho, 2, 18, 18, 28);     // Sala 4 (Mid-Left)
    desenharSala(desenho, 40, 18, 58, 28);    // Sala 5 (Center)
    desenharSala(desenho, 80, 18, 96, 28);    // Sala 6 (Mid-Right)

    // Row 3 rooms
    desenharSala(desenho, 2, 36, 18, 46);     // Sala 7 (Bottom-Left)
    desenharSala(desenho, 40, 36, 58, 46);    // Sala 8 (Bottom-Middle)
    desenharSala(desenho, 80, 36, 96, 46);    // Sala 9 (Bottom-Right)

    // Connect rooms horizontally
    // Row 1
    desenharCorredorHorizontal(desenho, 18, 40, 6);
    desenharCorredorHorizontal(desenho, 58, 80, 6);
    // Row 2
    desenharCorredorHorizontal(desenho, 18, 40, 23);
    desenharCorredorHorizontal(desenho, 58, 80, 23);
    // Row 3
    desenharCorredorHorizontal(desenho, 18, 40, 41);
    desenharCorredorHorizontal(desenho, 58, 80, 41);

    // Connect rooms vertically
    // Col 1
    desenharCorredorVertical(desenho, 10, 10, 18);
    desenharCorredorVertical(desenho, 10, 28, 36);
    // Col 2
    desenharCorredorVertical(desenho, 49, 10, 18);
    desenharCorredorVertical(desenho, 49, 28, 36);
    // Col 3
    desenharCorredorVertical(desenho, 88, 10, 18);
    desenharCorredorVertical(desenho, 88, 28, 36);

    // Place key landmarks based on level
    if (nivel == 1) {
        colocarElemento(desenho, 10, 14, CODIGO_PAREDE); // Bloqueio do NPC
        colocarElemento(desenho, 49, 14, CODIGO_ARMADILHA);
        colocarElemento(desenho, 80, 41, CODIGO_PORTA); // door to Room 9
        colocarElemento(desenho, 90, 41, CODIGO_SAIDA); // stairs in Room 9
    } else if (nivel == 2) {
        colocarElemento(desenho, 10, 14, CODIGO_PAREDE); // Bloqueio do NPC
        colocarElemento(desenho, 49, 14, CODIGO_ARMADILHA);
        colocarElemento(desenho, 10, 32, CODIGO_ARMADILHA);
        colocarElemento(desenho, 80, 41, CODIGO_PORTA);
        colocarElemento(desenho, 90, 41, CODIGO_SAIDA);
    } else {
        colocarElemento(desenho, 10, 14, CODIGO_PAREDE); // Bloqueio do NPC
        colocarElemento(desenho, 49, 14, CODIGO_ARMADILHA);
        colocarElemento(desenho, 10, 32, CODIGO_ARMADILHA);
        colocarElemento(desenho, 88, 32, CODIGO_ARMADILHA);
        colocarElemento(desenho, 80, 41, CODIGO_PORTA);
        colocarElemento(desenho, 90, 41, CODIGO_SAIDA);
    }

    return desenho;
}
} // namespace

void carregarNivel(Jogo& jogo, int nivel) {
    gerarMapa(jogo.mapa, nivel);
    criarInimigos(jogo.mapa, nivel);
    criarItens(jogo.mapa, nivel);
    criarNPCs(jogo.mapa, nivel);
    posicionarJogador(jogo);
    atualizarCampoDeVisao(jogo);
}

void gerarMapa(Mapa& mapa, int nivel) {
    mapa.nivelAtual = nivel;
    mapa.inimigos.clear();
    mapa.itens.clear();
    mapa.npcs.clear();

    vector<vector<int> > desenho = criarDesenhoNivel(nivel);
    aplicarLayout(mapa, desenho);
}

void posicionarJogador(Jogo& jogo) {
    jogo.jogador.posicao = {5, 5};
    jogo.jogador.x = jogo.jogador.posicao.x * 80.0f + 40.0f;
    jogo.jogador.y = jogo.jogador.posicao.y * 80.0f + 40.0f;
}

void criarInimigos(Mapa& mapa, int nivel) {
    if (nivel == 1) {
        mapa.inimigos.push_back(criarInimigo(49, 6, INIMIGO_ALEATORIO, nivel));
        mapa.inimigos.push_back(criarInimigo(49, 23, INIMIGO_PERSEGUIDOR, nivel));
        mapa.inimigos.push_back(criarInimigo(10, 41, INIMIGO_ALEATORIO, nivel));
    } else if (nivel == 2) {
        mapa.inimigos.push_back(criarInimigo(49, 6, INIMIGO_ALEATORIO, nivel));
        mapa.inimigos.push_back(criarInimigo(49, 23, INIMIGO_PERSEGUIDOR, nivel));
        mapa.inimigos.push_back(criarInimigo(49, 41, INIMIGO_TANQUE, nivel));
        mapa.inimigos.push_back(criarInimigo(88, 6, INIMIGO_ALEATORIO, nivel));
    } else {
        mapa.inimigos.push_back(criarInimigo(49, 23, INIMIGO_PERSEGUIDOR, nivel));
        mapa.inimigos.push_back(criarInimigo(49, 41, INIMIGO_TANQUE, nivel));
        mapa.inimigos.push_back(criarInimigo(88, 6, INIMIGO_PERSEGUIDOR, nivel));
        mapa.inimigos.push_back(criarInimigo(90, 41, INIMIGO_BOSS, nivel));
    }
}

void criarItens(Mapa& mapa, int nivel) {
    if (nivel == 1) {
        mapa.itens.push_back(criarItem(10, 8, ITEM_POCAO, 18));
        mapa.itens.push_back(criarItem(88, 4, ITEM_CHAVE, 1));
        mapa.itens.push_back(criarItem(10, 25, ITEM_ESCUDO, 1 + nivel));
        mapa.itens.push_back(criarItem(49, 25, ITEM_ARMA, 2 + nivel));
        mapa.itens.push_back(criarItem(10, 43, ITEM_POWERUP, 1));
    } else if (nivel == 2) {
        mapa.itens.push_back(criarItem(10, 8, ITEM_POCAO, 18));
        mapa.itens.push_back(criarItem(88, 4, ITEM_CHAVE, 1));
        mapa.itens.push_back(criarItem(88, 25, ITEM_ESCUDO, 1 + nivel));
        mapa.itens.push_back(criarItem(49, 25, ITEM_ARMA, 2 + nivel));
        mapa.itens.push_back(criarItem(49, 43, ITEM_POWERUP, 1));
    } else {
        mapa.itens.push_back(criarItem(10, 8, ITEM_POCAO, 18));
        mapa.itens.push_back(criarItem(88, 4, ITEM_CHAVE, 1));
        mapa.itens.push_back(criarItem(88, 25, ITEM_ESCUDO, 1 + nivel));
        mapa.itens.push_back(criarItem(49, 25, ITEM_ARMA, 2 + nivel));
        mapa.itens.push_back(criarItem(49, 43, ITEM_POWERUP, 1));
    }
}

void criarNPCs(Mapa& mapa, int nivel) {
    NPC NPC;
    NPC.posicao = nivel == 1 ? Posicao{10, 6} : Posicao{10, 23};
    NPC.nome = nivel == 1 ? "Guardiao" : "Ancia";
    NPC.fala = nivel == 1
        ? "Uma passagem se abriu ao sul. Use chaves nas portas."
        : "O labirinto muda para quem conversa com os antigos.";
    NPC.interagiu = false;
    mapa.npcs.push_back(NPC);
}

bool posicaoValida(const Mapa& mapa, Posicao posicao) {
    return posicao.x >= 0 && posicao.y >= 0 && posicao.x < mapa.largura && posicao.y < mapa.altura;
}

bool existeParedeOuBloqueio(const Mapa& mapa, Posicao posicao) {
    if (!posicaoValida(mapa, posicao)) {
        return true;
    }
    return mapa.tiles[posicao.y][posicao.x].bloqueiaMovimento;
}

bool dentroDoRaioDeVisao(Posicao origem, Posicao alvo, int raio) {
    int deslocamentoX = origem.x - alvo.x;
    int deslocamentoY = origem.y - alvo.y;
    return deslocamentoX * deslocamentoX + deslocamentoY * deslocamentoY <= raio * raio;
}

void atualizarCampoDeVisao(Jogo& jogo) {
    for (int y = 0; y < jogo.mapa.altura; y++) {
        for (int x = 0; x < jogo.mapa.largura; x++) {
            Posicao alvo = {x, y};
            bool visivel = dentroDoRaioDeVisao(jogo.jogador.posicao, alvo, RAIO_VISAO);
            jogo.mapa.tiles[y][x].visivel = visivel;
            if (visivel) {
                jogo.mapa.tiles[y][x].explorado = true;
            }
        }
    }
}

Inimigo* buscarInimigoNaPosicao(Mapa& mapa, Posicao posicao) {
    for (size_t i = 0; i < mapa.inimigos.size(); i++) {
        if (mapa.inimigos[i].vivo && mesmaPosicao(mapa.inimigos[i].posicao, posicao)) {
            return &mapa.inimigos[i];
        }
    }
    return nullptr;
}

Item* buscarItemNaPosicao(Mapa& mapa, Posicao posicao) {
    for (size_t i = 0; i < mapa.itens.size(); i++) {
        if (!mapa.itens[i].coletado && mesmaPosicao(mapa.itens[i].posicao, posicao)) {
            return &mapa.itens[i];
        }
    }
    return nullptr;
}

NPC* buscarNPCNaPosicao(Mapa& mapa, Posicao posicao) {
    for (size_t i = 0; i < mapa.npcs.size(); i++) {
        if (mesmaPosicao(mapa.npcs[i].posicao, posicao)) {
            return &mapa.npcs[i];
        }
    }
    return nullptr;
}

void alterarMapaPorNPC(Jogo& jogo, NPC& NPC) {
    if (NPC.interagiu) {
        jogo.mensagem = NPC.nome + ": Ja ajudei voce.";
        return;
    }

    NPC.interagiu = true;
    jogo.mensagem = NPC.nome + ": " + NPC.fala;
    Posicao passagem = {10, 14};
    if (posicaoValida(jogo.mapa, passagem)) {
        jogo.mapa.tiles[passagem.y][passagem.x] = criarTile(CHAO);
        jogo.mapa.matriz[passagem.y][passagem.x] = CODIGO_CHAO;
    }
}

void alterarMapaPorEvento(Jogo& jogo, Posicao posicao) {
    if (!posicaoValida(jogo.mapa, posicao)) {
        return;
    }
    if (jogo.mapa.tiles[posicao.y][posicao.x].tipo == ARMADILHA) {
        jogo.mapa.tiles[posicao.y][posicao.x] = criarTile(CHAO);
        jogo.mapa.matriz[posicao.y][posicao.x] = CODIGO_CHAO;
    }
}
