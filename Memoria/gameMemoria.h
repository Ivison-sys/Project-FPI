#ifndef GAMEMEMORIA_H
#define GAMEMEMORIA_H
#include "../lib/raylib.h"

typedef enum{
    VERSO,
    FRENTE,
    ENCONTRADA
}StateCard;

typedef enum{
    MENU,
    PEDINDO_NOME_J1,
    PEDINDO_NOME_J2,
    JOGANDO,
    PAUSADO,
    VITORIA
} StateGame;

typedef struct {
    StateCard estado;
    Rectangle retangulo;
    Texture2D logo;
    char* nomeImg;
}Card;

// Funcoes principais
void drawCards(Card** cards);
Card** inicilizandoCards();
Card** buscandoClick(Card** cards);
char** gerandoSeq();
void gameMemoria();

// Variaveis globais dos jogadores
#define MAX_NOME_JOGADOR 50
extern char nomeJogador1[MAX_NOME_JOGADOR];
extern char nomeJogador2[MAX_NOME_JOGADOR];
extern int pontuacaoJogador1;
extern int pontuacaoJogador2; 
extern int jogadorAtual;

#endif