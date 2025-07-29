#ifndef GAMEHOQUEI_H
#define GAMEHOQUEI_H

#include "../lib/raylib.h"

typedef struct Jogador{
    Vector2 posicao;
    Vector2 velocidade;
    Vector2 iniciogol;
    Vector2 fimgol;
    float vel_max;
    float raio;
    int gols;
}Jogador;

typedef struct Bola{
    Vector2 posicao;
    Vector2 velocidade;
    float vel_max;
}Bola;






void gameHoquei();

#endif