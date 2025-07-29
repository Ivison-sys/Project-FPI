#ifndef GAMEHOQUEI_H
#define GAMEHOQUEI_H

#include "../lib/raylib.h"

typedef struct Jogador{
    Vector2 posicao;
    Vector2 velocidade;
    Color cor;
    float raio;
    int gols;
}Jogador;

typedef struct Bola{
    Vector2 posicao;
    Vector2 velocidade;
    Color cor;
    float raio;
    
}Bola;






void gameHoquei();

#endif