#ifndef GAMEMEMORIA_H
#define GAMEMEMORIA_H
#include "../lib/raylib.h"

typedef enum{
    VERSO,
    FRENTE,
}StateCard;

typedef struct {
    StateCard estado;
    Rectangle retangulo;
    Texture2D logo;
}Card;

void drawCards(Card** cards);

Card** inicilizandoCards();

Card** buscandoClick(Card** cards);

void gameMemoria();
#endif