#ifndef GAMEHOQUEI_H
#define GAMEHOQUEI_H

#include "../lib/raylib.h"

typedef struct Jogador{
    Vector2 posicao;
    Vector2 velocidade;
    Vector2 iniciogol;
    int gols = 0;
}Jogador;

typedef struct Bola{
    Vector2 posicao;
    Vector2 velocidade;
}Bola;




void gameHoquei();

#endif