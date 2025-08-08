#ifndef GAMEMEMORIA_H
#define GAMEMEMORIA_H
#include "../Lib/raylib.h"

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
    char* nome;
    bool bugUsado;
}Card;

// funcoes principais
void drawCards(Card** cards);
Card** inicilizandoCards();
Card** buscandoClick(Card** cards);
char** gerandoSeq();
void gameMemoria();

// sons
void carregandoSons();
void liberandoSons();
void controleSom();

// bug
void mostraBug();

// jogadores
#define MAX_NOME_JOGADOR 50
extern char nomeJogador1[MAX_NOME_JOGADOR];
extern char nomeJogador2[MAX_NOME_JOGADOR];
extern int pts1;
extern int pts2; 
extern int turno;

// sons
extern Sound musicaFundo;
extern Sound somOk;
extern Sound somFalha;
extern Sound somFlip;
extern Sound somFim;
extern Sound somBug;
extern bool tocando;

// bug
extern bool bugAtivo;
extern float bugTempo;

#endif