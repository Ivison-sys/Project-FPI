// main.c
#include <raylib.h>
#include "gameGolf.h" // Inclui nosso módulo de jogo

int main(void)
{
    // Inicialização da Janela e Áudio
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Jogo Refatorado - Sem Globais");
    InitAudioDevice();
    SetTargetFPS(90);
    Texture2D texturaGrama = LoadTexture("piso_gramado.png");

    // Cria as estruturas para o estado e recursos do jogo
    Game game = {0};
    GameAssets assets = {0};

    // Carrega os recursos e inicializa o estado do jogo
    CarregarRecursos(&assets);
    InicializarJogo(&game);
    
    // Loop Principal do Jogo
    while (!WindowShouldClose())
    
    {
        // ATUALIZAÇÃO
        AtualizarJogo(&game, &assets);
        

        DrawTexture(texturaGrama, 0, 0, WHITE);
        
        // DESENHO
        BeginDrawing();
            DesenharJogo(&game);

        EndDrawing();
    }

    // Descarregamento e Finalização
    DescarregarRecursos(&assets);
    UnloadTexture(texturaGrama);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}