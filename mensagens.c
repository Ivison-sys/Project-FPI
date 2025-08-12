#include "Mensagens/mensagens.h"
#include "Golf/gameGolf.h"
#include "Hoquei/gameHoquei.h"
#include "Memoria/gameMemoria.h"
#include <stdio.h>

static int (*obterJogo(int id))(void) {
    switch(id) {
        case 1: return gameGolf;
        case 2: return gameHoquei; 
        case 3: return gameMemoria;
        default: return gameGolf;
    }
}

static void mostrarTela(const char* mensagem, int dudu_wins, int leo_wins, bool eh_final) {
    InitWindow(800, 600, "Msgs");
    InitAudioDevice();
    SetTargetFPS(60);
    
    // Carrega os sons
    Sound som_intermediario = LoadSound("music/Intermediario.mp3");
    Sound som_final = LoadSound("music/Final.mp3");
    
    // Reproduz o som apropriado
    if (eh_final) {
        PlaySound(som_final);
    } else {
        PlaySound(som_intermediario);
    }
    
    float tempo = 0.0f;
    const float DURACAO = 4.0f;
    
    while (!WindowShouldClose() && tempo < DURACAO) {
        tempo += GetFrameTime();
        
        BeginDrawing();
        ClearBackground((Color){0, 0, 0, 255});
        
        // Placar
        char placar[50];
        sprintf(placar, "DUDU %d x %d LÉO", dudu_wins, leo_wins);
        int largura_placar = MeasureText(placar, 40);
        DrawText(placar, 400 - largura_placar/2, 150, 40, WHITE);
        
        // Mensagem
        int largura_msg = MeasureText(mensagem, 24);
        DrawText(mensagem, 400 - largura_msg/2, 300, 24, LIGHTGRAY);
        
        EndDrawing();
    }
    
    // Libera os recursos de áudio
    UnloadSound(som_intermediario);
    UnloadSound(som_final);
    CloseAudioDevice();
    CloseWindow();
}

int executarMsgs(int ordem_jogos[3]) {
    int placar[2] = {0, 0}; // [dudu, leo]
    int jogo_atual = 0;
    
    while (placar[0] < 2 && placar[1] < 2 && jogo_atual < 3) {
        // Executa o jogo
        int resultado = obterJogo(ordem_jogos[jogo_atual])();
        
        if (resultado == 0) break; 
        
        // Atualiza placar
        placar[resultado - 1]++;
        jogo_atual++;
        
        // Escolhe mensagem e determina se é final
        const char* mensagem = "";
        bool eh_final = false;
        
        if (placar[0] == 1 && placar[1] == 0) {
            mensagem = "O primeiro jogo ficou com DUDU, ele está mais próximo dos 50 reais";
        } else if (placar[0] == 0 && placar[1] == 1) {
            mensagem = "O primeiro jogo ficou com LÉO, ele está mais próximo dos 50 reais";
        } else if ((placar[0] == 1 && placar[1] == 1)) {
            char* vencedor = (resultado == 1) ? "DUDU" : "LÉO";
            static char msg_recuperacao[150];
            sprintf(msg_recuperacao, "Agora sim, %s se recupera e leva essa para mais um jogo", vencedor);
            mensagem = msg_recuperacao;
        } else if (placar[0] == 2 && jogo_atual == 2) {
            mensagem = "Com uma vitória esmagadora de 2 a 0 o DUDU leva os 50 reais";
            eh_final = true;
        } else if (placar[1] == 2 && jogo_atual == 2) {
            mensagem = "Com uma vitória esmagadora de 2 a 0 o LÉO leva os 50 reais";
            eh_final = true;
        } else if (placar[0] == 2) {
            mensagem = "Essa decisão foi acirrada, teve que ir até o último jogo, e com essa última vitória o DUDU garante os 50 reais";
            eh_final = true;
        } else if (placar[1] == 2) {
            mensagem = "Essa decisão foi acirrada, teve que ir até o último jogo, e com essa última vitória o LÉO garante os 50 reais";
            eh_final = true;
        }
        
        // Mostra tela de resultado
        mostrarTela(mensagem, placar[0], placar[1], eh_final);
    }
    
    //

    // Retorna vencedor (1=Dudu, 2=Léo)
    return (placar[0] > placar[1]) ? 1 : 2;
}