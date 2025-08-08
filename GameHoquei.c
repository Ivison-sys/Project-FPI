#include "Lib/raylib.h"
#include "Hoquei/gameHoquei.h"

void gameHoquei(){
    InitWindow(800, 600, "Jogo da Hoquei");
    SetTargetFPS(60);
    int isFim = 0;

    while(!isFim){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hoquei", GetScreenHeight()/2, GetScreenWidth()/2, 50, BLACK);
        if(IsKeyPressed(KEY_ENTER)){
            isFim = 1;
        }
        EndDrawing();
    }
    CloseWindow();
}