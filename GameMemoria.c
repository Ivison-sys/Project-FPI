#include "Memoria/gameMemoria.h"
#include "Lib/raylib.h"
#include "Lib/raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define dim 60
#define qtdCards 49
#define margin 6

StateGame estadoAtual = MENU;

char** gerandoSeq(){
    char* nomes[] = {"arduino", "assembly", "c", "cpp", "csharp", "css", "dart", "fortran", "git",
                    "github", "go", "html", "java", "js", "kotlin", "lua", "php",  "python", "quartus", 
                    "r", "react", "swift", "ts", "vscode", "arduino", "assembly", "c", "cpp", "csharp", 
                    "css", "dart", "fortran", "git", "github", "go", "html", "java", "js", "kotlin", 
                    "lua", "php",  "python", "quartus", "r", "react", "swift", "ts", "vscode"
                };
    char** seq = malloc(49 * sizeof(char*));
    for(int i = 47; i > 0; i--){
        int j = rand() % (i+1);

        char* temp = nomes[i];
        nomes[i] = nomes[j];
        nomes[j] = temp;
    }
    for(int i = 0; i < 48; i++){
        seq[i] = nomes[i];
    }


    char* aux = "ruby";
    int j = rand() % 48;
    char* temp = seq[j];
    seq[j] = aux;
    seq[48] = temp;

    return seq;

}

Card** inicilizandoCards(){

    Card** cards = (Card**) malloc(qtdCards * sizeof(Card*));
    int raiz = sqrt(qtdCards);
    char** seg = gerandoSeq();
    Vector2 inicio = (Vector2){(GetScreenWidth() - dim)/2 - (dim + margin) * (raiz / 2), (GetScreenHeight() - dim)/2 - (dim + margin) * (raiz / 2)};
    for(int i = 0, x= inicio.x, y= inicio.y; i < qtdCards; i++, x+= dim + margin){
        if(i % raiz == 0){
            x = inicio.x;
            y = inicio.y + ((dim + margin) * (i/raiz));
        }

        cards[i] = (Card*) malloc(sizeof(Card));

        char* icon = seg[i];
        char caminho[250];

        snprintf(caminho, sizeof(caminho), "Memoria/src/imgs/%s.png", icon);
        Image img = LoadImage(caminho);
        ImageResize(&img, dim, dim);
        cards[i]->logo = LoadTextureFromImage(img);
        UnloadImage(img);
        cards[i]->estado = VERSO;
        cards[i]->retangulo = (Rectangle){x, y, dim, dim};
    }
    
    return cards;
}

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

Card** buscandoClick(Card** cards){
    Vector2 mouse =  GetMousePosition();
    for(int i = 0; i < qtdCards; i++){
        if(CheckCollisionPointRec(mouse, cards[i]->retangulo)){
            cards[i]->estado = (cards[i]->estado == VERSO) ? FRENTE : VERSO;
        }
    }
    return cards;
}

// Funções dos estados
void DrawMenu(){
    DrawText("Este é o menu do jogo, pressione enter para começar!", 10, 10, 20, BLACK);
}

void DrawPausado(Card** cards){
    drawCards(cards);
    DrawText("O jogo está pausado, pressione enter para retorna!", 10, 10, 20, BLUE);
}

Card** drawJogando(Card** cards){
    drawCards(cards);
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) cards = buscandoClick(cards);
    return cards;
}



void gameMemoria(){
    InitWindow(800, 600, "Jogo da mémoria");
    SetTargetFPS(60);
    srand(time(NULL));
    
    Card** cards = inicilizandoCards();
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        switch(estadoAtual){
            case MENU:
                DrawMenu();
                if(IsKeyPressed(KEY_ENTER))  estadoAtual = JOGANDO;
            break;

            case JOGANDO:
                cards = drawJogando(cards);
                if(IsKeyPressed(KEY_ENTER)) estadoAtual = PAUSADO;
            break;

            case PAUSADO:
                DrawPausado(cards);
                if(IsKeyPressed(KEY_ENTER)) estadoAtual = JOGANDO;
            break;
        }
        EndDrawing();
    }
    CloseWindow();
}