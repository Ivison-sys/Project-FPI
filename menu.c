#include "Lib/raylib.h"
#include "Menu/gameMenu.h"
#include "Sorteio/sorteio.h"
#include "Mensagens/mensagens.h"

int menu(){
    //----------------------------------------------------------------------------------
    // Inicialização
    //----------------------------------------------------------------------------------
    const int screenWidth = 1680;
    const int screenHeight = 940;

    InitWindow(screenWidth, screenHeight, "Meu Jogo de Aventura");
    InitAudioDevice();

    Sound narracao = LoadSound("Menu/sons/narracao.mp3"); 
    bool narracaoIniciada = false;
    GameScreen currentScreen = TELA_HISTORIA;
    
    // --- ALTERADO: Apenas a variável de animação dos créditos é necessária ---
    float creditosOffsetY = screenHeight;

    // --- Variáveis da Tela de História ---
    const char *historia[] = {
        "Dois amigos, Léo e Dudu, voltavam para casa depois da escola, quando, no meio do caminho, algo brilhou no chão. ",
        "Era uma nota de 50 reais, novinha, reluzente, quase mágica. Os dois se entreolharam. Quem viu primeiro ? Quem devia ficar com ela ?",
        "",
        "-Vamos dividir — disse Léo, tentando parecer justo.",
        "-Dividir ? Eu que vi primeiro! - Rebateu Dudu, fechando o punho.",
        "",
        "Mas nenhum dos dois queria ceder. E foi então que surgiu a ideia brilhante — ou completamente maluca.",
        "",
        "-Vamos resolver isso como adultos civilizados... em um duelo de minigames!",
        "",
        "Três desafios foram propostos. Quem vencesse dois dos três jogos, ficaria com toda a nota.",
        " O outro ? Faria o perdedor pagar um sorvete no caminho de volta  - Com o próprio dinheiro!"
    };
    int numLinhasHistoria = sizeof(historia) / sizeof(historia[0]);
    float scrollY = screenHeight;
    float velocidadeScroll = 25.0f;
    int fontSizeHistoria = 20;
    int espacamentoLinhas = 30;

    // --- NOVO: Texto dos créditos organizado em um array ---
    const char *creditos[] = {
        "CREDITOS",
        "",
        "Jogo criado por:",
        "Marcos Antonio",
        "Erick Barros",
        "Ivison Rafael",
        "Durian Fernades",
        "Iago Lopes",
        "Pedro Henrique",
        "",
        "------------------------------------",
        "",
        "Golf e Menu: Marcos Antonio e Erick Barros",
        "Memoria e Juncao dos Jogos: Ivison Rafael e Durian Fernades",
        "Hoquei e Sorteio dos Jogos: Iago Lopes e Pedro Henrique"
    };
    int numLinhasCreditos = sizeof(creditos) / sizeof(creditos[0]);
    int fontSizeCreditos = 20;
    int espacamentoCreditos = 35;

    // --- Variáveis da Tela de Menu ---
    Rectangle botaoIniciar = { (float)screenWidth / 2 - 120, 250, 240, 50 };
    Rectangle botaoCreditos = { (float)screenWidth / 2 - 120, 350, 240, 50 };
    Rectangle botaoSair = { (float)screenWidth / 2 - 120, 450, 240, 50 };

    SetTargetFPS(60);

    //----------------------------------------------------------------------------------
    // Loop Principal do Jogo
    //----------------------------------------------------------------------------------
    while (!WindowShouldClose() && currentScreen != TELA_SAIR) {
        //----------------------------------------------------------------------------------
        // Atualização (Lógica)
       
    
    switch (currentScreen) {
        case TELA_HISTORIA: {
            // Inicia a narração apenas uma vez
            if (!narracaoIniciada) {
                PlaySound(narracao);
                narracaoIniciada = true;
            }

       
            
            // Lógica de scroll e pulo
            scrollY -= velocidadeScroll * GetFrameTime();
            if (scrollY < -(numLinhasHistoria * espacamentoLinhas) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (narracaoIniciada) {
                    StopSound(narracao);
                }
                currentScreen = TELA_MENU;
            }
        } break;

            case TELA_MENU: {
                // --- ALTERADO: Lógica estática original, sem animação ---
                Vector2 mousePoint = GetMousePosition();
                if (CheckCollisionPointRec(mousePoint, botaoIniciar) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentScreen = TELA_JOGO;
                if (CheckCollisionPointRec(mousePoint, botaoCreditos) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    currentScreen = TELA_CREDITOS;
                    creditosOffsetY = screenHeight; // Reseta a animação dos créditos
                }
                if (CheckCollisionPointRec(mousePoint, botaoSair) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentScreen = TELA_SAIR;
            } break;

            case TELA_CREDITOS: {
                // Lógica de animação para os créditos
                if (creditosOffsetY  > 0) {
                    creditosOffsetY -= (creditosOffsetY / 10.0f) + 1.0f;
                    if (creditosOffsetY < 0) creditosOffsetY = 0;
                }
                
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                    currentScreen = TELA_MENU;
                }
            } break;

            case TELA_JOGO: {
                if (IsKeyPressed(KEY_ESCAPE)) {
                    currentScreen = TELA_MENU;
                }
            } break;

            default: break;
        }

        //----------------------------------------------------------------------------------
        // Desenho (Renderização)
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentScreen) {
            case TELA_HISTORIA: {
                for (int i = 0; i < numLinhasHistoria; i++) {
                    float textWidth = MeasureText(historia[i], fontSizeHistoria);
                    DrawText(historia[i], (float)screenWidth / 2 - textWidth / 2, scrollY + i * espacamentoLinhas, fontSizeHistoria, WHITE);
                }
            } break;

            case TELA_MENU: {
                // --- ALTERADO: Desenho estático original, sem animação ---
                DrawText("A GUERRA DOS 50!", (int)(screenWidth / 2 - MeasureText("A GUERRA DOS 50!", 40) / 2), 100, 40, WHITE);
                Vector2 mousePoint = GetMousePosition();
                DrawRectangleRec(botaoIniciar, CheckCollisionPointRec(mousePoint, botaoIniciar) ? SKYBLUE : WHITE);
                DrawText("Iniciar Jogo", (int)botaoIniciar.x + 45, (int)botaoIniciar.y + 15, 20, BLACK);
                DrawRectangleRec(botaoCreditos, CheckCollisionPointRec(mousePoint, botaoCreditos) ? SKYBLUE : WHITE);
                DrawText("Creditos", (int)botaoCreditos.x + 75, (int)botaoCreditos.y + 15, 20, BLACK);
                DrawRectangleRec(botaoSair, CheckCollisionPointRec(mousePoint, botaoSair) ? SKYBLUE : WHITE);
                DrawText("Sair", (int)botaoSair.x + 95, (int)botaoSair.y + 15, 20, BLACK);
            } break;

            case TELA_CREDITOS: {
                // --- ALTERADO: Desenho animado e organizado dos créditos ---
                float yPos = 100 + creditosOffsetY; // Posição Y inicial + offset da animação

                for (int i = 0; i < numLinhasCreditos; i++) {
                    int currentFontSize = (i == 0) ? 20 : fontSizeCreditos; // Título maior
                    float textWidth = MeasureText(creditos[i], currentFontSize);
                    DrawText(creditos[i], (float)screenWidth / 2 - textWidth / 2, yPos, currentFontSize, WHITE);
                    
                    yPos += (i == 0) ? espacamentoCreditos * 1.3f : espacamentoCreditos; // Espaço maior depois do título
                }

                if (creditosOffsetY == 0) {
                     DrawText("Pressione [Enter] para voltar", (int)(screenWidth / 2 - MeasureText("Pressione [Enter] para voltar", 20) / 2), screenHeight - 60, 20, WHITE);
                }
            } break;

            case TELA_JOGO: {
                EndDrawing();
                UnloadSound(narracao); 
                CloseAudioDevice();
                CloseWindow();
                return 1;
            } break;

            default: break;
        }

        EndDrawing();   

    }

    //----------------------------------------------------------------------------------
    // Finalização
    //----------------------------------------------------------------------------------
     UnloadSound(narracao); 
     CloseAudioDevice();
     CloseWindow();
     return 0;
}
