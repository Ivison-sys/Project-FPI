// menu.h
#ifndef MENU_H
#define MENU_H
#include "lib/raylib.h"  

typedef enum GameScreen {
    TELA_HISTORIA,
    TELA_MENU,
    TELA_CREDITOS,
    TELA_JOGO,
    TELA_SAIR
} GameScreen;


void menu();

#endif // MENU_H