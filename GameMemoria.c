#include "Memoria/gameMemoria.h"
#include "Lib/raylib.h"
#include "Lib/raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define dim 60
#define qtdCards 50
#define margin 6

StateGame estadoAtual = MENU;

// controle do jogo
Card* primeira = NULL;
Card* segunda = NULL;
bool esperando = false;
float tempo = 1.0f;
int pares = 0;

// jogadores
char nomeJogador1[MAX_NOME_JOGADOR] = "Jogador 1";
char nomeJogador2[MAX_NOME_JOGADOR] = "Jogador 2";
int pts1 = 0;
int pts2 = 0;
int turno = 1;

char input[MAX_NOME_JOGADOR + 1] = "\0";
int numChars = 0;

// sons
Sound musicaFundo;
Sound somOk;
Sound somFalha;
Sound somFlip;
Sound somFim;
Sound somBug;
bool tocando = false;

//bug 
bool bugAtivo = false;
float bugTempo = 0.0f;

void carregandoSons(){
    InitAudioDevice();
    
    musicaFundo = LoadSound("Memoria/src/music/Fundo.mp3");
    somOk = LoadSound("Memoria/src/music/Sucesso.mp3");
    somFalha = LoadSound("Memoria/src/music/Erro.mp3");
    somFlip = LoadSound("Memoria/src/music/Carta.mp3");
    somFim = LoadSound("Memoria/src/music/Final_Jogo.mp3");
    somBug = LoadSound("Memoria/src/music/Bug.mp3");
}

void liberandoSons(){
    UnloadSound(musicaFundo);
    UnloadSound(somOk);
    UnloadSound(somFalha);
    UnloadSound(somFlip);
    UnloadSound(somFim);
    UnloadSound(somBug);
    CloseAudioDevice();
}

void controleSom(){
    if(!tocando && estadoAtual != VITORIA && !bugAtivo){
        PlaySound(musicaFundo);
        tocando = true;
    }
    
    if(tocando && !IsSoundPlaying(musicaFundo) && estadoAtual != VITORIA && !bugAtivo){
        PlaySound(musicaFundo);
    }
    
    if(tocando && (estadoAtual == VITORIA || bugAtivo)){
        StopSound(musicaFundo);
        tocando = false;
    }
}



char** gerandoSeq(){
    char* nomes[] = {"arduino", "assembly", "c", "cpp", "csharp", "css", "dart", "fortran", "git",
                    "github", "go", "html", "java", "js", "kotlin", "lua", "php",  "python", "quartus", 
                    "r", "react", "swift", "ts", "vscode", "arduino", "assembly", "c", "cpp", "csharp", 
                    "css", "dart", "fortran", "git", "github", "go", "html", "java", "js", "kotlin", 
                    "lua", "php",  "python", "quartus", "r", "react", "swift", "ts", "vscode"
                };
    
    // Add bugs
    char* temp[50];
    for(int i = 0; i < 48; i++){
        temp[i] = nomes[i];
    }
    temp[48] = "Bug";
    temp[49] = "Bug";
    
    char** seq = malloc(50 * sizeof(char*));
    

    for(int i = 49; i > 0; i--){
        int j = rand() % (i+1);
        char* aux = temp[i];
        temp[i] = temp[j];
        temp[j] = aux;
    }
    
    for(int i = 0; i < 50; i++){
        seq[i] = temp[i];
    }

    return seq;
}

Card** inicilizandoCards(){
    Card** cards = (Card**) malloc(qtdCards * sizeof(Card*));
    int cols = 10;
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
        cards[i]->nome = strdup(icon);
        cards[i]->estado = VERSO;
        cards[i]->bugUsado = false;
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
        if(CheckCollisionPointRec(mouse, cards[i]->retangulo) && cards[i]->estado == VERSO && !bugAtivo){
            cards[i]->estado = FRENTE;
            PlaySound(somFlip);
            
            //   se clicou em bug
            if(strcmp(cards[i]->nome, "Bug") == 0 && !cards[i]->bugUsado){
                cards[i]->bugUsado = true;
                cards[i]->estado = ENCONTRADA;
                bugAtivo = true;
                bugTempo = 7.0f;
                PlaySound(somBug);
                StopSound(musicaFundo);
                tocando = false;
                
                turno = (turno == 1) ? 2 : 1;
                
                if(primeira != NULL) {
                    primeira->estado = VERSO;
                    primeira = NULL;
                }
                if(segunda != NULL) {
                    segunda->estado = VERSO;
                    segunda = NULL;
                }
                esperando = false;
                break;
            }
            
            if(primeira == NULL){
                primeira = cards[i];
            } else if(segunda == NULL){
                if(cards[i] == primeira){
                    cards[i]->estado = VERSO;
                    primeira = NULL;
                    continue;
                }
                segunda = cards[i];
                esperando = true;
                tempo = 1.0f;
            }
            break;
        }
    }
    return cards;
}

void mostraBug(){
    if(bugAtivo && bugTempo > 0.0f){
        float fade = bugTempo / 7.0f; 
        
        char* msg = "DEU BUG! você perdeu a vez";
        int w = MeasureText(msg, 40);
        int x = GetScreenWidth()/2 - w/2;
        int y = GetScreenHeight()/2 - 20;
        
        DrawRectangle(x - 20, y - 10, w + 40, 60, ColorAlpha(DARKGRAY, fade * 0.8f));
        DrawText(msg, x, y, 40, ColorAlpha(RED, fade));
    }
}

// Estados
void DrawMenu(){
    DrawText("JOGO DA MEMÓRIA", GetScreenWidth()/2 - MeasureText("JOGO DA MEMÓRIA", 50)/2, GetScreenHeight()/2 - 100, 50, BLACK);
    DrawText("Pressione enter para começar!", GetScreenWidth()/2 - MeasureText("Pressione enter para começar!", 20)/2, GetScreenHeight()/2 + 50, 20, RED);
}

void DrawInputNome(const char* prompt){
    DrawText(prompt, GetScreenWidth()/2 - MeasureText(prompt, 30)/2, GetScreenHeight()/2 - 80, 30, BLACK);
    
    DrawRectangle(GetScreenWidth()/2 - 150, GetScreenHeight()/2 - 20, 300, 40, LIGHTGRAY);
    DrawRectangleLines(GetScreenWidth()/2 - 150, GetScreenHeight()/2 - 20, 300, 40, DARKGRAY);
    
    Color cor = (estadoAtual == PEDINDO_NOME_J1) ? BLACK : RED;
    DrawText(input, GetScreenWidth()/2 - 140, GetScreenHeight()/2 - 10, 20, cor);
    
    if(((int)(GetTime() * 2)) % 2 == 0) DrawText("_", GetScreenWidth()/2 - 140 + MeasureText(input, 20), GetScreenHeight()/2 - 10, 20, cor);
    
    DrawText("Pressione enter para confirmar", GetScreenWidth()/2 - MeasureText("Pressione enter para confirmar", 20)/2, GetScreenHeight()/2 + 60, 20, BLACK);
}

void processaTexto(char* dest, int max, StateGame proximo){
    int key = GetCharPressed();
    while(key > 0){
        if((key >= 32) && (key <= 125) && (numChars < max)){
            input[numChars] = (char)key;
            numChars++;
            input[numChars] = '\0';
        }
        key = GetCharPressed();
    }
    
    if(IsKeyPressed(KEY_BACKSPACE)){
        if(numChars > 0){
            numChars--;
            input[numChars] = '\0';
        }
    }
    
    if(IsKeyPressed(KEY_ENTER)){
        if(numChars > 0){
            strcpy(dest, input);
            numChars = 0;
            input[0] = '\0';
            estadoAtual = proximo;
        }
    }
}

void DrawPausado(Card** cards){
    drawCards(cards);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(WHITE, 0.7f));
    
    char* txt = "O jogo está pausado, pressione enter para retorna!";
    int w = MeasureText(txt, 20);
    int x = GetScreenWidth()/2 - w/2;
    int y = GetScreenHeight()/2 - 10;
    
    DrawRectangle(x - 10, y - 5, w + 20, 30, ColorAlpha(WHITE, 0.8f));
    DrawText(txt, x, y, 20, BLACK);
}

Card** drawJogando(Card** cards){
    drawCards(cards);
    
    //bug timer
    if(bugAtivo){
        bugTempo -= GetFrameTime();
        if(bugTempo <= 0.0f){
            bugAtivo = false;
            bugTempo = 0.0f;
        }
    }
    
    // UI
    DrawText(TextFormat("%s: %d", nomeJogador1, pts1), 10, 10, 20, BLACK);
    DrawText(TextFormat("%s: %d", nomeJogador2, pts2), GetScreenWidth() - MeasureText(TextFormat("%s: %d", nomeJogador2, pts2), 20) - 10, 10, 20, RED);
    DrawText(TextFormat("Turno: %s", (turno == 1) ? nomeJogador1 : nomeJogador2), GetScreenWidth()/2 - MeasureText(TextFormat("Turno: %s", (turno == 1) ? nomeJogador1 : nomeJogador2), 25)/2, 10, 25, BLACK);
    
    // logica comparacao
    if(esperando && !bugAtivo){
        tempo -= GetFrameTime();
        
        if(tempo <= 0.0f){
            if(primeira != NULL && segunda != NULL){
                if(strcmp(primeira->nome, segunda->nome) == 0){
                    primeira->estado = ENCONTRADA;
                    segunda->estado = ENCONTRADA;
                    pares++;
                    PlaySound(somOk);
                    
                    if(turno == 1){
                        pts1++;
                    } else {
                        pts2++;
                    }
                } else {
                    primeira->estado = VERSO;
                    segunda->estado = VERSO;
                    PlaySound(somFalha);
                    turno = (turno == 1) ? 2 : 1;
                }
            }
            primeira = NULL;
            segunda = NULL;
            esperando = false;
        }
    } else if(!bugAtivo) {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) cards = buscandoClick(cards);
    }
    
    mostraBug();
    
    if(pares == 24){ 
        estadoAtual = VITORIA;
        PlaySound(somFim);
    }
    
    return cards;
}

void DrawVitoria(){
    const char* winner;
    if(pts1 > pts2){
        winner = nomeJogador1;
    } else if(pts2 > pts1){
        winner = nomeJogador2;
    } else {
        winner = "Empate!";
    }
    
    DrawText("FIM DE JOGO!", GetScreenWidth()/2 - MeasureText("FIM DE JOGO!", 50)/2, GetScreenHeight()/2 - 120, 50, BLACK);
    DrawText(TextFormat("Vencedor: %s", winner), GetScreenWidth()/2 - MeasureText(TextFormat("Vencedor: %s", winner), 30)/2, GetScreenHeight()/2 - 60, 30, BLACK);
    DrawText(TextFormat("%s: %d", nomeJogador1, pts1), GetScreenWidth()/2 - MeasureText(TextFormat("%s: %d", nomeJogador1, pts1), 25)/2, GetScreenHeight()/2, 25, BLACK);
    DrawText(TextFormat("%s: %d", nomeJogador2, pts2), GetScreenWidth()/2 - MeasureText(TextFormat("%s: %d", nomeJogador2, pts2), 25)/2, GetScreenHeight()/2 + 40, 25, RED);
    
    DrawText("Pressione 'R' para jogar novamente", GetScreenWidth()/2 - MeasureText("Pressione 'R' para jogar novamente", 20)/2, GetScreenHeight()/2 + 100, 20, BLACK);
}

void gameMemoria(){
    InitWindow(800, 600, "Jogo da mémoria");
    SetTargetFPS(60);
    srand(time(NULL));
    
    carregandoSons();
    
    Card** cards = NULL;
    
    while(!WindowShouldClose()){
        controleSom();
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        switch(estadoAtual){
            case MENU:
                DrawMenu();
                if(IsKeyPressed(KEY_ENTER)) estadoAtual = PEDINDO_NOME_J1;
            break;
            
            case PEDINDO_NOME_J1:
                DrawInputNome("Digite o nome do Primeiro Jogador:");
                processaTexto(nomeJogador1, MAX_NOME_JOGADOR - 1, PEDINDO_NOME_J2);
            break;
            
            case PEDINDO_NOME_J2:
                DrawInputNome("Digite o nome do Segundo Jogador:");
                processaTexto(nomeJogador2, MAX_NOME_JOGADOR - 1, JOGANDO);
                if(estadoAtual == JOGANDO && cards == NULL){
                    cards = inicilizandoCards();
                    pts1 = 0;
                    pts2 = 0;
                    pares = 0;
                    turno = 1;
                    bugAtivo = false;
                    bugTempo = 0.0f;
                }
            break;

            case JOGANDO:
                cards = drawJogando(cards);
                if(IsKeyPressed(KEY_ENTER) && !bugAtivo) estadoAtual = PAUSADO;
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
                            free(cards[i]->nome);
                            free(cards[i]);
                        }
                        free(cards);
                        cards = NULL;
                    }
                    
                    primeira = NULL;
                    segunda = NULL;
                    esperando = false;
                    tempo = 1.0f;
                    pares = 0;
                    pts1 = 0;
                    pts2 = 0;
                    turno = 1;
                    bugAtivo = false;
                    bugTempo = 0.0f;
                    
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
            free(cards[i]->nome);
            free(cards[i]);
        }
        free(cards);
    }
    
    liberandoSons();
    CloseWindow();
}