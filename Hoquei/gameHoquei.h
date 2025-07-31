#ifndef GAMEHOQUEI_H
#define GAMEHOQUEI_H

#include "../lib/raylib.h"

typedef struct Jogador{
    Vector2 posicao;
    Vector2 velocidade;
    Vector2 atracao;
    Color cor;
    float raio;
    float coefgravidade;
    int gols;
}Jogador;

typedef struct Bola{
    Vector2 posicao;
    Vector2 velocidade;
    Color cor;
    Sound som;
    float raio;
    
}Bola;

typedef struct Gol{
    Vector2 inicio;
    Vector2 fim;
    Color cor;
}Gol;

typedef struct Divisoria{
    Vector2 inicio;
    Vector2 fim;
    Color cor;
}Divisoria;

typedef struct Circulo{
    Vector2 posicao;
    float raio;
    Color cor;
}Circulo;



typedef struct GameState{
    Jogador jogador1;
    Jogador jogador2;
    Bola bola;
    Gol gol1;
    Gol gol2;
    Divisoria divisoria;
    Circulo circuloesq, circulomeio, circulodir;
    float volumemusicadefundo, aceleracao, hue, atrito, vel_max, vel_max_bola, opacidade, opacidadefade, sobreposicao, proj, G;
    int screen_width, screen_height;
    int flagcontador, flagmusica, rodando, terminou, contador, grossuralinha, fps;
    double tempoatual;
    char tempo[20], placar[6], textfps[10], contregressiva[4], vencedor[20];
    Color colorbackground, colortext, colormidline, fadecolor;
    Music musicadefundo;
    Sound somcontagem;
    Vector2 vetorcolisao;
    Vector2 n;
    Vector2 v_rel;
    Vector2 variacao;

}GameState;

void iniciarJogo(GameState *game);

void atualizaCoresSom(GameState *game);

void controlarJogadores (GameState *game);

void atualizarContador (GameState *game);

void verificarColisoes (GameState *game);

void desenharJogo (GameState *game);

void animacaoFinal (GameState *game);

void gameHoquei();

#endif