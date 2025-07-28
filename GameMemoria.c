#include "Memoria/gameMemoria.h"
#include "Lib/raylib.h"
#include "Lib/raymath.h"
#include <stdio.h>
#include <stdlib.h>
#define dim 60
#define qtdCards 49
#define margin 6

void drawCards(Card** cards){
    for(int i = 0; i < qtdCards; i++){
        switch (cards[i]->estado){
            case VERSO:
                DrawRectangleRec(cards[i]->retangulo, RED);
            break;

            case FRENTE:
               DrawTexture(cards[i]->logo, cards[i]->retangulo.x, cards[i]->retangulo.y, WHITE);
            break;
        }
    }
}

Card** inicilizandoCards(){
    Card** cards = (Card**) malloc(qtdCards * sizeof(Card*));
    int raiz = sqrt(qtdCards);
    Vector2 inicio = (Vector2){(GetScreenWidth() - dim)/2 - (dim + margin) * (raiz / 2), (GetScreenHeight() - dim)/2 - (dim + margin) * (raiz / 2)};
    for(int i = 0, x= inicio.x, y= inicio.y; i < qtdCards; i++, x+= dim + margin){
        if(i % raiz == 0){
            x = inicio.x;
            y = inicio.y + ((dim + margin) * (i/raiz));
        }

        cards[i] = (Card*) malloc(sizeof(Card));
        Image img = LoadImage("Memoria/src/imgs/python.jpg");
        ImageResize(&img, dim, dim);
        cards[i]->logo = LoadTextureFromImage(img);
        UnloadImage(img);
        cards[i]->estado = VERSO;
        cards[i]->retangulo = (Rectangle){x, y, dim, dim};
    }

    return cards;
}

Card** buscandoClick(Card** cards){
    Vector2 mouse =  GetMousePosition();
    for(int i = 0; i < qtdCards; i++){
        if(CheckCollisionPointRec(mouse, cards[i]->retangulo)){
            cards[i]->estado = (cards[i]->estado == VERSO) ? FRENTE : VERSO;
        }
    }
    return cards;
}

void gameMemoria(){
    InitWindow(800, 600, "Jogo da mémoria");
    SetTargetFPS(60);
    
    Card** cards = inicilizandoCards();
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        drawCards(cards);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) cards = buscandoClick(cards);
        EndDrawing();
    }
    CloseWindow();
}