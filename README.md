# CC-Rogue-Like-Game

Projeto para a disciplina de Algoritmos do curso de Ciência da Computação da UNIVALI, com o objetivo de desenvolver um game rogue like.

## 🎮 Sobre o Projeto
Este é um jogo 2D top-down estilo Rogue-like desenvolvido em **C++** utilizando a biblioteca **SFML**. 
O projeto conta com um sistema de renderização de mapas exportados do **Tiled Map Editor** (formato JSON/.tmj), garantindo colisões precisas e efeitos de profundidade (Foreground/Z-Sorting).

## 🏗️ Estrutura do Projeto
A arquitetura foi pensada de forma modular e profissional, separando o código por domínio de responsabilidade:

- **`src/main.cpp`**: O coração do jogo. Controla o Loop Principal (Game Loop), a Câmera Dinâmica (Clamp) e a renderização geral na janela.
- **`src/entities/`**: Classes de entidades do jogo. Contém a classe `Player.cpp` responsável pela lógica do Herói, inputs do teclado, máquina de estados (idle, walk, attack) e atributos de RPG (Vida, XP).
- **`src/graphics/TileMap.cpp`**: Motor de renderização do mapa. Utiliza `nlohmann/json` para parsear o arquivo `.tmj` do Tiled e monta a geometria da fase de forma otimizada usando `sf::VertexArray`. Suporta Camadas de Frente (efeito 3D) e Caixas de Colisão Precisas (Object Layers).
- **`src/world/`**: Gerenciamento de estado do mundo e carregamento das fases (`Level.cpp`).
- **`src/assets/`**: Imagens, texturas e sprites do jogo.
- **`CMakeLists.txt`**: Orquestrador de build automático. Baixa a SFML dinamicamente, linka as dependências e copia os Assets/Mapas para a pasta do executável final.

## 🚀 Como Rodar o Jogo

Este projeto utiliza o **CMake** para gerenciar a compilação. Não é necessário instalar a SFML manualmente no sistema, o CMake fará o download da versão correta (2.6.1) durante o primeiro build.

### Pré-requisitos
- Um compilador C++20 (GCC/MinGW no Windows, Clang no Mac, ou MSVC).
- CMake (versão 3.24 ou superior).
- IDE Recomendada: **CLion** (ou VSCode com as extensões C++ e CMake Tools).

### Passo a Passo (CLion)
1. Abra a pasta raiz do projeto (`CC-Rogue-Like-Game`) no CLion.
2. O CLion detectará automaticamente o `CMakeLists.txt` e iniciará a configuração. Caso não inicie, clique com o botão direito no `CMakeLists.txt` e selecione **"Reload CMake Project"**.
3. O CMake baixará a SFML do GitHub. Aguarde a finalização.
4. Selecione a configuração de Build (ex: `CC_Rogue_Like_Game | Debug`) no menu superior.
5. Clique no botão de **Play** (Run) verde.
6. O CMake irá compilar o executável, gerar as DLLs necessárias e **copiar as pastas `src/assets` e `src/world`** automaticamente para a pasta de saída (ex: `cmake-build-debug`).
7. Divirta-se!

## 🗺️ Como Editar o Mapa
O mapa do jogo é gerado pela ferramenta gratuita [Tiled](https://www.mapeditor.org/).
Para criar ou modificar o mapa:
1. Abra o arquivo `src/world/mapa_lvl_1.tmj` no Tiled.
2. Pinte o cenário utilizando as Camadas de Blocos.
   - **Camadas Normais ("Chão")**: Desenhadas atrás do jogador.
   - **Camadas de Frente ("Frente", "Topo")**: Desenhadas sobre a cabeça do jogador (Efeito 3D).
3. Para colisões, crie uma **Camada de Objetos (Object Layer)** contendo a palavra `"Colisao"` e use a ferramenta de **Retângulo (R)** para desenhar caixas matemáticas precisas em volta dos obstáculos.
4. Salve o arquivo (`Ctrl+S`) e rode o jogo. O C++ processará o JSON do mapa automaticamente!
