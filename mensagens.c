#include "Mensagens/mensagens.h"
#include "Golf/gameGolf.h"
#include "Hoquei/gameHoquei.h"
#include "Memoria/gameMemoria.h"
#include "Menu/gameMenu.h"
#include "Sorteio/sorteio.h"
#include <stdio.h>

static int (*obterJogo(int id))(void) {
    switch(id) {
        case 1: return gameGolf;
        case 2: return gameHoquei; 
        case 3: return gameMemoria;
        default: return gameGolf;
    }
}

static void mostrarTelaIntermediaria(const char* mensagem, int dudu_wins, int leo_wins) {
    InitWindow(800, 600, "Msgs");
    SetTargetFPS(60);
    
    float tempo = 0.0f;
    const float DURACAO = 10.0f; 
    bool forcou_saida = false;
    
    // Preparar strings e medidas fora do loop
    char placar[50];
    sprintf(placar, "DUDU %d x %d LÉO", dudu_wins, leo_wins);
    int largura_placar = MeasureText(placar, 40);
    int largura_msg = MeasureText(mensagem, 24);
    const char* instrucao = "Pressione ENTER ou ESPAÇO para continuar";
    int largura_inst = MeasureText(instrucao, 16);
    
    while (!WindowShouldClose() && tempo < DURACAO && !forcou_saida) {
        tempo += GetFrameTime();
        
        // Verifica se o jogador quer pular
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            forcou_saida = true;
        }
        
        BeginDrawing();
        ClearBackground((Color){0, 0, 0, 255});
        
        // Calcular alpha para fade-out
        float progresso = tempo / DURACAO;
        int alpha = (int)(255 * (1.0f - progresso * 0.8f)); 
        if (alpha < 32) alpha = 32; 
        
        //
        DrawText(placar, 400 - largura_placar/2, 150, 40, (Color){255, 255, 255, alpha});
        
        //
        DrawText(mensagem, 400 - largura_msg/2, 300, 24, (Color){190, 190, 190, alpha});
        
        //
        DrawText(instrucao, 400 - largura_inst/2, 450, 16, (Color){128, 128, 128, alpha});
        
        EndDrawing();
    }
    
    CloseWindow();
}
 
static int mostrarPlacarFinal(const char* mensagem, int dudu_wins, int leo_wins) {
    InitWindow(800, 600, "Msgs - Placar Final");
    SetTargetFPS(60);
    
    bool opcao_escolhida = false;
    int escolha = 0; // 0 =não escolheu, 1 =jogar novamente
    
    while (!WindowShouldClose() && !opcao_escolhida) {
        
        if (IsKeyPressed(KEY_ENTER)) {
            escolha = 1; 
            opcao_escolhida = true;
        }
        
        BeginDrawing();
        ClearBackground((Color){0, 0, 0, 255});
        

        //Titulo
        const char* titulo = "RESULTADO FINAL";
        int largura_titulo = MeasureText(titulo, 48);
        DrawText(titulo, 400 - largura_titulo/2, 80, 48, GOLD);
        
        // Placar final
        char placar[50];
        sprintf(placar, "DUDU %d x %d LÉO", dudu_wins, leo_wins);
        int largura_placar = MeasureText(placar, 44);
        DrawText(placar, 400 - largura_placar/2, 180, 44, WHITE);
        
        // Verifica se a mensagem é muito longa e quebra em duas linhas

        const char* linha1 = "";
        const char* linha2 = "";
        
        if (strlen(mensagem) > 60) {
            if (strstr(mensagem, "acirrada")) {
                linha1 = "Essa decisão foi acirrada, teve que ir até o último jogo,";
                if (strstr(mensagem, "DUDU")) {
                    linha2 = "e com essa última vitória o DUDU garante os 50 reais";
                } else {
                    linha2 = "e com essa última vitória o LÉO garante os 50 reais";
                }
            } else {
                linha1 = mensagem; 
                linha2 = "";
            }
        } else {
            linha1 = mensagem;
            linha2 = "";
        }
        
        int largura_linha1 = MeasureText(linha1, 22);
        DrawText(linha1, 400 - largura_linha1/2, 280, 22, LIGHTGRAY);
        
        if (strlen(linha2) > 0) {
            int largura_linha2 = MeasureText(linha2, 22);
            DrawText(linha2, 400 - largura_linha2/2, 310, 22, LIGHTGRAY);
        }
        

        
        const char* inst1 = "Aperte ENTER para jogar novamente";
        int largura_inst1 = MeasureText(inst1, 20);
        DrawText(inst1, 400 - largura_inst1/2, 415, 20, GREEN);
        
        EndDrawing();
    }
    
    CloseWindow();
    
    return escolha;
}

int executarMsgs(int ordem_jogos[3]) {
    int placar[2] = {0, 0}; // [dudu / leo]
    int jogo_atual = 0;
    
    while (placar[0] < 2 && placar[1] < 2 && jogo_atual < 3) {
        //executa o jogo
        int resultado = obterJogo(ordem_jogos[jogo_atual])();
        
        if (resultado == 0) break; 
        
        // Atualiza placar
        placar[resultado - 1]++;
        jogo_atual++;
        
        //Escolhe mensagem e determina se é final
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
        
        //placar

        if (eh_final) {
            int escolha = mostrarPlacarFinal(mensagem, placar[0], placar[1]);
            
            if (escolha == 1) {
                // Reiniciar o jogo 
                if(menu()) {
                   
                    sorteia_ordem_ids(ordem_jogos);
                    
                    tela_sorteio_preview(ordem_jogos);
                    
                    //Resetat variáveis e recomeçar
                    placar[0] = 0;
                    placar[1] = 0;
                    jogo_atual = 0;
                    continue; // Reinicia o loop principal
                } else {
                    break; // Sai se o menu retornar false
                }
            } else {
                break; // Escolheu sair ou fechou a janela
            }
        } else {

            mostrarTelaIntermediaria(mensagem, placar[0], placar[1]);
        }
    }
    

    //Retorna vencedor 1=Dudu  2=Léo
    if (placar[0] == placar[1]) return 0;
    return (placar[0] > placar[1]) ? 1 : 2;
}