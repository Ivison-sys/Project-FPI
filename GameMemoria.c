#include "Memoria/gameMemoria.h"
#include "Lib/raylib.h"
#include "Lib/raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define dim 60
#define qtdCards 48
#define margin 6

StateGame estadoAtual = MENU;

// Variaveis de controle do jogo
Card* primeiraCartaVirada = NULL;
Card* segundaCartaVirada = NULL;
bool aguardandoComparacao = false;
float tempoParaVirarDeVolta = 1.0f;
int paresEncontrados = 0;

// Variaveis dos jogadores
char nomeJogador1[MAX_NOME_JOGADOR] = "Jogador 1";
char nomeJogador2[MAX_NOME_JOGADOR] = "Jogador 2";
int pontuacaoJogador1 = 0;
int pontuacaoJogador2 = 0;
int jogadorAtual = 1;

// Variaveis de input de texto
char textoInput[MAX_NOME_JOGADOR + 1] = "\0";
int numCaracteresInput = 0;

char** gerandoSeq(){
    char* nomesUnicos[] = {
        "arduino", "assembly", "c", "cpp", "csharp", "css", "dart", "fortran", "git",
        "github", "go", "html", "java", "js", "kotlin", "lua", "php", "python", "quartus", 
        "r", "react", "swift", "ts", "vscode" 
    };
    int numIconesUnicos = sizeof(nomesUnicos) / sizeof(nomesUnicos[0]);
    int totalDeCartas = numIconesUnicos * 2;
    
    char** seq = malloc(totalDeCartas * sizeof(char*));
    
    for(int i = 0; i < numIconesUnicos; i++){
        seq[i * 2] = nomesUnicos[i];
        seq[i * 2 + 1] = nomesUnicos[i];
    }
    
    for(int i = totalDeCartas - 1; i > 0; i--){
        int j = rand() % (i + 1);
        char* temp = seq[i];
        seq[i] = seq[j];
        seq[j] = temp;
    }
    
    return seq;
}

Card** inicilizandoCards(){
    Card** cards = (Card**) malloc(qtdCards * sizeof(Card*));
    int cols = 8;
    char** seq = gerandoSeq();
    
    Vector2 inicio = (Vector2){
        (GetScreenWidth() - (dim * cols + margin * (cols - 1))) / 2,
        (GetScreenHeight() - (dim * (qtdCards / cols) + margin * ((qtdCards / cols) - 1))) / 2
    };
    
    for(int i = 0; i < qtdCards; i++){
        int col = i % cols;
        int row = i / cols;
        
        cards[i] = (Card*) malloc(sizeof(Card));
        
        char* icon = seq[i];
        char caminho[250];
        
        snprintf(caminho, sizeof(caminho), "Memoria/src/imgs/%s.png", icon);
        Image img = LoadImage(caminho);
        ImageResize(&img, dim, dim);
        cards[i]->logo = LoadTextureFromImage(img);
        UnloadImage(img);
        cards[i]->nomeImg = strdup(icon);
        cards[i]->estado = VERSO;
        cards[i]->retangulo = (Rectangle){
            inicio.x + col * (dim + margin),
            inicio.y + row * (dim + margin),
            dim, dim
        };
    }
    
    free(seq);
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
            
            case ENCONTRADA:
            DrawRectangleRec(cards[i]->retangulo, LIGHTGRAY);
            DrawTexture(cards[i]->logo, cards[i]->retangulo.x, cards[i]->retangulo.y, ColorAlpha(WHITE, 0.7f));
            break;
        }
        DrawRectangleLinesEx(cards[i]->retangulo, 2, BLACK);
    }
}

Card** buscandoClick(Card** cards){
    Vector2 mouse = GetMousePosition();
    for(int i = 0; i < qtdCards; i++){
        if(CheckCollisionPointRec(mouse, cards[i]->retangulo) && cards[i]->estado == VERSO){
            cards[i]->estado = FRENTE;
            
            if(primeiraCartaVirada == NULL){
                primeiraCartaVirada = cards[i];
            } else if(segundaCartaVirada == NULL){
                if(cards[i] == primeiraCartaVirada){
                    cards[i]->estado = VERSO;
                    primeiraCartaVirada = NULL;
                    continue;
                }
                segundaCartaVirada = cards[i];
                aguardandoComparacao = true;
                tempoParaVirarDeVolta = 1.0f;
            }
            break;
        }
    }
    return cards;
}

// Funcoes dos estados
void DrawMenu(){
    DrawText("JOGO DA MEMÓRIA", GetScreenWidth()/2 - MeasureText("JOGO DA MEMÓRIA", 50)/2, GetScreenHeight()/2 - 100, 50, BLACK);
    DrawText("Pressione enter para começar!", GetScreenWidth()/2 - MeasureText("Pressione enter para começar!", 20)/2, GetScreenHeight()/2 + 50, 20, RED);
}

void DrawInputNome(const char* prompt){
    DrawText(prompt, GetScreenWidth()/2 - MeasureText(prompt, 30)/2, GetScreenHeight()/2 - 80, 30, BLACK);
    
    DrawRectangle(GetScreenWidth()/2 - 150, GetScreenHeight()/2 - 20, 300, 40, LIGHTGRAY);
    DrawRectangleLines(GetScreenWidth()/2 - 150, GetScreenHeight()/2 - 20, 300, 40, DARKGRAY);
    
    // Cores dos jogadores: primeiro preto/negrito, segundo vermelho/negrito
    Color corTexto = (estadoAtual == PEDINDO_NOME_J1) ? BLACK : RED;
    DrawText(textoInput, GetScreenWidth()/2 - 140, GetScreenHeight()/2 - 10, 20, corTexto);
    
    if(((int)(GetTime() * 2)) % 2 == 0) DrawText("_", GetScreenWidth()/2 - 140 + MeasureText(textoInput, 20), GetScreenHeight()/2 - 10, 20, corTexto);
    
    DrawText("Pressione enter para confirmar", GetScreenWidth()/2 - MeasureText("Pressione enter para confirmar", 20)/2, GetScreenHeight()/2 + 60, 20, BLACK);
}

void processandoTexto(char* destBuffer, int maxLen, StateGame proximoEstado){
    int key = GetCharPressed();
    while(key > 0){
        if((key >= 32) && (key <= 125) && (numCaracteresInput < maxLen)){
            textoInput[numCaracteresInput] = (char)key;
            numCaracteresInput++;
            textoInput[numCaracteresInput] = '\0';
        }
        key = GetCharPressed();
    }
    
    if(IsKeyPressed(KEY_BACKSPACE)){
        if(numCaracteresInput > 0){
            numCaracteresInput--;
            textoInput[numCaracteresInput] = '\0';
        }
    }
    
    if(IsKeyPressed(KEY_ENTER)){
        if(numCaracteresInput > 0){
            strcpy(destBuffer, textoInput);
            numCaracteresInput = 0;
            textoInput[0] = '\0';
            estadoAtual = proximoEstado;
        }
    }
}

void DrawPausado(Card** cards){
    // Efeito de desfoque - desenhando sobre as cartas com transparencia
    drawCards(cards);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(WHITE, 0.7f));
    
    // Fundo levemente esbranquicado para o texto de pause centralizado
    char* textoPause = "O jogo está pausado, pressione enter para retorna!";
    int larguraTexto = MeasureText(textoPause, 20);
    int posX = GetScreenWidth()/2 - larguraTexto/2;
    int posY = GetScreenHeight()/2 - 10;
    
    DrawRectangle(posX - 10, posY - 5, larguraTexto + 20, 30, ColorAlpha(WHITE, 0.8f));
    
    // Texto de pause em preto e centralizado
    DrawText(textoPause, posX, posY, 20, BLACK);
}

Card** drawJogando(Card** cards){
    drawCards(cards);
    
    // UI dos jogadores com cores fixas
    DrawText(TextFormat("%s: %d", nomeJogador1, pontuacaoJogador1), 10, 10, 20, BLACK);
    DrawText(TextFormat("%s: %d", nomeJogador2, pontuacaoJogador2), GetScreenWidth() - MeasureText(TextFormat("%s: %d", nomeJogador2, pontuacaoJogador2), 20) - 10, 10, 20, RED);
    DrawText(TextFormat("Turno: %s", (jogadorAtual == 1) ? nomeJogador1 : nomeJogador2), GetScreenWidth()/2 - MeasureText(TextFormat("Turno: %s", (jogadorAtual == 1) ? nomeJogador1 : nomeJogador2), 25)/2, 10, 25, BLACK);
    
    // Logica de comparacao
    if(aguardandoComparacao){
        tempoParaVirarDeVolta -= GetFrameTime();
        
        if(tempoParaVirarDeVolta <= 0.0f){
            if(primeiraCartaVirada != NULL && segundaCartaVirada != NULL){
                if(strcmp(primeiraCartaVirada->nomeImg, segundaCartaVirada->nomeImg) == 0){
                    primeiraCartaVirada->estado = ENCONTRADA;
                    segundaCartaVirada->estado = ENCONTRADA;
                    paresEncontrados++;
                    
                    if(jogadorAtual == 1){
                        pontuacaoJogador1++;
                    } else {
                        pontuacaoJogador2++;
                    }
                } else {
                    primeiraCartaVirada->estado = VERSO;
                    segundaCartaVirada->estado = VERSO;
                    jogadorAtual = (jogadorAtual == 1) ? 2 : 1;
                }
            }
            primeiraCartaVirada = NULL;
            segundaCartaVirada = NULL;
            aguardandoComparacao = false;
        }
    } else {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) cards = buscandoClick(cards);
    }
    
    if(paresEncontrados == (qtdCards / 2)){
        estadoAtual = VITORIA;
    }
    
    return cards;
}

void DrawVitoria(){
    const char* vencedorNome;
    if(pontuacaoJogador1 > pontuacaoJogador2){
        vencedorNome = nomeJogador1;
    } else if(pontuacaoJogador2 > pontuacaoJogador1){
        vencedorNome = nomeJogador2;
    } else {
        vencedorNome = "Empate!";
    }
    
    DrawText("FIM DE JOGO!", GetScreenWidth()/2 - MeasureText("FIM DE JOGO!", 50)/2, GetScreenHeight()/2 - 120, 50, BLACK);
    DrawText(TextFormat("Vencedor: %s", vencedorNome), GetScreenWidth()/2 - MeasureText(TextFormat("Vencedor: %s", vencedorNome), 30)/2, GetScreenHeight()/2 - 60, 30, BLACK);
    DrawText(TextFormat("%s: %d", nomeJogador1, pontuacaoJogador1), GetScreenWidth()/2 - MeasureText(TextFormat("%s: %d", nomeJogador1, pontuacaoJogador1), 25)/2, GetScreenHeight()/2, 25, BLACK);
    DrawText(TextFormat("%s: %d", nomeJogador2, pontuacaoJogador2), GetScreenWidth()/2 - MeasureText(TextFormat("%s: %d", nomeJogador2, pontuacaoJogador2), 25)/2, GetScreenHeight()/2 + 40, 25, RED);
    
    DrawText("Pressione 'R' para jogar novamente", GetScreenWidth()/2 - MeasureText("Pressione 'R' para jogar novamente", 20)/2, GetScreenHeight()/2 + 100, 20, BLACK);
}

void gameMemoria(){
    InitWindow(800, 600, "Jogo da mémoria");
    SetTargetFPS(60);
    srand(time(NULL));
    
    Card** cards = NULL;
    
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        switch(estadoAtual){
            case MENU:
                DrawMenu();
                if(IsKeyPressed(KEY_ENTER)) estadoAtual = PEDINDO_NOME_J1;
            break;
            
            case PEDINDO_NOME_J1:
                DrawInputNome("Digite o nome do Primeiro Jogador:");
                processandoTexto(nomeJogador1, MAX_NOME_JOGADOR - 1, PEDINDO_NOME_J2);
            break;
            
            case PEDINDO_NOME_J2:
                DrawInputNome("Digite o nome do Segundo Jogador:");
                processandoTexto(nomeJogador2, MAX_NOME_JOGADOR - 1, JOGANDO);
                if(estadoAtual == JOGANDO && cards == NULL){
                    cards = inicilizandoCards();
                    pontuacaoJogador1 = 0;
                    pontuacaoJogador2 = 0;
                    paresEncontrados = 0;
                    jogadorAtual = 1;
                }
            break;

            case JOGANDO:
                cards = drawJogando(cards);
                if(IsKeyPressed(KEY_ENTER)) estadoAtual = PAUSADO;
            break;

            case PAUSADO:
                DrawPausado(cards);
                if(IsKeyPressed(KEY_ENTER)) estadoAtual = JOGANDO;
            break;
            
            case VITORIA:
                DrawVitoria();
                if(IsKeyPressed(KEY_R)){
                    if(cards != NULL){
                        for(int i = 0; i < qtdCards; i++){
                            UnloadTexture(cards[i]->logo);
                            free(cards[i]->nomeImg);
                            free(cards[i]);
                        }
                        free(cards);
                        cards = NULL;
                    }
                    
                    primeiraCartaVirada = NULL;
                    segundaCartaVirada = NULL;
                    aguardandoComparacao = false;
                    tempoParaVirarDeVolta = 1.0f;
                    paresEncontrados = 0;
                    pontuacaoJogador1 = 0;
                    pontuacaoJogador2 = 0;
                    jogadorAtual = 1;
                    
                    strcpy(nomeJogador1, "Jogador 1");
                    strcpy(nomeJogador2, "Jogador 2");
                    
                    estadoAtual = MENU;
                }
            break;
        }
        EndDrawing();
    }
    
    if(cards != NULL){
        for(int i = 0; i < qtdCards; i++){
            UnloadTexture(cards[i]->logo);
            free(cards[i]->nomeImg);
            free(cards[i]);
        }
        free(cards);
    }
    CloseWindow();
}