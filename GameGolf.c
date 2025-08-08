#include "Lib/raylib.h"
#include "Golf/gameGolf.h"

void gameGolf(){
    InitWindow(800, 600, "Jogo da golf");
    SetTargetFPS(60);
    int isFim = 0;

    while(!isFim){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Golf", GetScreenWidth()/2, GetScreenHeight()/2, 50, BLACK);
        if(IsKeyPressed(KEY_ENTER)){
            isFim = 1;
        }
        EndDrawing();
    }
    CloseWindow();

}