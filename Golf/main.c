// main.c
#include <raylib.h>
#include "golf.h" // Inclui nosso módulo de jogo

int main(void)
{
    // Inicialização da Janela e Áudio
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Jogo Refatorado - Sem Globais");
    InitAudioDevice();
    SetTargetFPS(90);

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
        
        // DESENHO
        BeginDrawing();
            DesenharJogo(&game);
        EndDrawing();
    }

    // Descarregamento e Finalização
    DescarregarRecursos(&assets);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}