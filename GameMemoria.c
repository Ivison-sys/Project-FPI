#include "Memoria/gameMemoria.h"
#include "Lib/raylib.h"
#include "Lib/raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constantes Internas
#define COLUNAS_GRID 10
#define TRANSPARENCIA_ENCONTRADA 0.7f
#define TRANSPARENCIA_PAUSE 0.7f
#define TRANSPARENCIA_BUG 0.8f
#define LARGURA_BORDA 2

// Paleta de Cores
static const Color CARTA_VERSO = {230, 41, 55, 255};
static const Color COR_CARTA_COMBINADA = LIGHTGRAY;
static const Color BORDA_CARTA = BLACK;
static const Color COR_PLAYER1 = BLACK;
static const Color COR_PLAYER2 = {230, 41, 55, 255};
static const Color FUNDO_TELA = RAYWHITE;
static const Color TEXTO_BUG = {230, 41, 55, 255};
static const Color FUNDO_BUG = DARKGRAY;

// Geração de Sequência de Cartas
static char** CriarSequenciaEmbaralhada(void)
{
    static char* lista_nomes[] = {
        "arduino", "assembly", "c", "cpp", "csharp", "css", "dart", "fortran", "git",
        "github", "go", "html", "java", "js", "kotlin", "lua", "php", "python", "quartus", 
        "r", "react", "swift", "ts", "vscode", "arduino", "assembly", "c", "cpp", "csharp", 
        "css", "dart", "fortran", "git", "github", "go", "html", "java", "js", "kotlin", 
        "lua", "php", "python", "quartus", "r", "react", "swift", "ts", "vscode"
    };
    
    char* buffer_temp[TOTAL_CARTAS];
    int i;
    for(i = 0; i < 48; i++) {
        buffer_temp[i] = lista_nomes[i];
    }
    buffer_temp[48] = "Bug";
    buffer_temp[49] = "Bug";
    
    // Embaralhamento
    for(i = TOTAL_CARTAS - 1; i > 0; i--) {
        int indice_aleatorio = rand() % (i + 1);
        char* temporario = buffer_temp[i];
        buffer_temp[i] = buffer_temp[indice_aleatorio];
        buffer_temp[indice_aleatorio] = temporario;
    }
    
    char** sequencia_final = malloc(TOTAL_CARTAS * sizeof(char*));
    for(i = 0; i < TOTAL_CARTAS; i++) {
        sequencia_final[i] = buffer_temp[i];
    }
    
    return sequencia_final;
}

// Inicialização das Cartas
static void ConfigurarCartasJogo(CartaJogo*** cartas_ptr)
{
    *cartas_ptr = malloc(TOTAL_CARTAS * sizeof(CartaJogo*));
    char** sequencia = CriarSequenciaEmbaralhada();
    
    Vector2 posicao_inicial = {
        (TELA_LARGURA - (TAMANHO_CARTA * COLUNAS_GRID + ESPACAMENTO_CARTA * (COLUNAS_GRID - 1))) / 2,
        (TELA_ALTURA - (TAMANHO_CARTA * (TOTAL_CARTAS / COLUNAS_GRID) + ESPACAMENTO_CARTA * ((TOTAL_CARTAS / COLUNAS_GRID) - 1))) / 2
    };
    
    int idx;
    for(idx = 0; idx < TOTAL_CARTAS; idx++) {
        int coluna = idx % COLUNAS_GRID;
        int linha = idx / COLUNAS_GRID;
        
        (*cartas_ptr)[idx] = malloc(sizeof(CartaJogo));
        
        char caminho_completo[250];
        snprintf(caminho_completo, sizeof(caminho_completo), "Memoria/src/imgs/%s.png", sequencia[idx]);
        
        Image img_carregada = LoadImage(caminho_completo);
        ImageResize(&img_carregada, TAMANHO_CARTA, TAMANHO_CARTA);
        (*cartas_ptr)[idx]->textura = LoadTextureFromImage(img_carregada);
        UnloadImage(img_carregada);
        
        (*cartas_ptr)[idx]->identificador = strdup(sequencia[idx]);
        (*cartas_ptr)[idx]->status = CARTA_VIRADA;
        (*cartas_ptr)[idx]->bug_ativado = false;
        (*cartas_ptr)[idx]->bounds = (Rectangle){
            posicao_inicial.x + coluna * (TAMANHO_CARTA + ESPACAMENTO_CARTA),
            posicao_inicial.y + linha * (TAMANHO_CARTA + ESPACAMENTO_CARTA),
            TAMANHO_CARTA, TAMANHO_CARTA
        };
    }
    
    free(sequencia);
}

//

static void LiberarCartasJogo(CartaJogo** cartas)
{
    if (cartas == NULL) return;
    
    int i;
    for(i = 0; i < TOTAL_CARTAS; i++) {
        if (cartas[i] != NULL) {
            UnloadTexture(cartas[i]->textura);
            free(cartas[i]->identificador);
            free(cartas[i]);
        }
    }
    free(cartas);
}

//Reset do Jogo
static void ResetarEstadoJogo(JogoMemoria *jogo)
{
    jogo->status = PARTIDA_ATIVA;
    jogo->turno_atual = 0;
    jogo->vencedor_final = -1;
    jogo->combinacoes_feitas = 0;
    
    jogo->participantes[0].pontos = 0;
    jogo->participantes[1].pontos = 0;
    
    jogo->comparador.primeira = NULL;
    jogo->comparador.segunda = NULL;
    jogo->comparador.processando = false;
    jogo->comparador.cronometro = DURACAO_COMPARACAO;
    
    jogo->bug_system.em_acao = false;
    jogo->bug_system.timer = 0.0f;
    
    if (jogo->conjunto_cartas != NULL) {
        LiberarCartasJogo(jogo->conjunto_cartas);
    }
    ConfigurarCartasJogo(&jogo->conjunto_cartas);
}

// Clique do Mouse
static void TratarCliqueUsuario(JogoMemoria *jogo)
{
    if (jogo->bug_system.em_acao || jogo->comparador.processando) return;
    
    Vector2 pos_mouse = GetMousePosition();
    
    int i;
    for(i = 0; i < TOTAL_CARTAS; i++) {
        CartaJogo *carta_atual = jogo->conjunto_cartas[i];
        
        if (CheckCollisionPointRec(pos_mouse, carta_atual->bounds) && carta_atual->status == CARTA_VIRADA) {
            PlaySound(jogo->gerenciador_audio.som_carta);
            
            // Tratamento especial para carta Bug
            if (strcmp(carta_atual->identificador, "Bug") == 0 && !carta_atual->bug_ativado) {
                carta_atual->bug_ativado = true;
                carta_atual->status = CARTA_COMBINADA;
                jogo->bug_system.em_acao = true;
                jogo->bug_system.timer = DURACAO_EFEITO_BUG;
                PlaySound(jogo->gerenciador_audio.som_bug);
                StopSound(jogo->gerenciador_audio.musica_principal);
                jogo->gerenciador_audio.audio_ativo = false;
                
                jogo->turno_atual = (jogo->turno_atual == 0) ? 1 : 0;
                
                if (jogo->comparador.primeira != NULL) {
                    jogo->comparador.primeira->status = CARTA_VIRADA;
                    jogo->comparador.primeira = NULL;
                }
                if (jogo->comparador.segunda != NULL) {
                    jogo->comparador.segunda->status = CARTA_VIRADA;
                    jogo->comparador.segunda = NULL;
                }
                jogo->comparador.processando = false;
                return;
            }
            

            if (jogo->comparador.primeira == NULL) {
                jogo->comparador.primeira = carta_atual;
            } else if (jogo->comparador.segunda == NULL) {
                if (carta_atual == jogo->comparador.primeira) {
                    carta_atual->status = CARTA_VIRADA;
                    jogo->comparador.primeira = NULL;
                    return;
                }
                jogo->comparador.segunda = carta_atual;
                jogo->comparador.processando = true;
                jogo->comparador.cronometro = DURACAO_COMPARACAO;
            }
            carta_atual->status = CARTA_REVELADA;
            return;
        }
    }
}


// Sistema de Comparação de Cartas
static void ExecutarComparacaoCartas(JogoMemoria *jogo)
{
    if (!jogo->comparador.processando || jogo->bug_system.em_acao) return;
    
    jogo->comparador.cronometro -= GetFrameTime();
    
    if (jogo->comparador.cronometro <= 0.0f) {
        if (jogo->comparador.primeira != NULL && jogo->comparador.segunda != NULL) {
            if (strcmp(jogo->comparador.primeira->identificador, jogo->comparador.segunda->identificador) == 0) {
                jogo->comparador.primeira->status = CARTA_COMBINADA;
                jogo->comparador.segunda->status = CARTA_COMBINADA;
                jogo->combinacoes_feitas++;
                PlaySound(jogo->gerenciador_audio.som_acerto);
                
                jogo->participantes[jogo->turno_atual].pontos++;
            } else {
                jogo->comparador.primeira->status = CARTA_VIRADA;
                jogo->comparador.segunda->status = CARTA_VIRADA;
                PlaySound(jogo->gerenciador_audio.som_erro);
                jogo->turno_atual = (jogo->turno_atual == 0) ? 1 : 0;
            }
        }
        
        jogo->comparador.primeira = NULL;
        jogo->comparador.segunda = NULL;
        jogo->comparador.processando = false;
    }
}

// Sistema do Bug
static void ProcessarSistemaBug(JogoMemoria *jogo)
{
    if (!jogo->bug_system.em_acao) return;
    
    jogo->bug_system.timer -= GetFrameTime();
    if (jogo->bug_system.timer <= 0.0f) {
        jogo->bug_system.em_acao = false;
        jogo->bug_system.timer = 0.0f;
    }
}


//Controle de Áudio
static void GerenciarAudio(JogoMemoria *jogo)
{
    if (!jogo->gerenciador_audio.audio_ativo && jogo->status != PARTIDA_FINALIZADA && !jogo->bug_system.em_acao) {
        PlaySound(jogo->gerenciador_audio.musica_principal);
        jogo->gerenciador_audio.audio_ativo = true;
    }
    
    if (jogo->gerenciador_audio.audio_ativo && !IsSoundPlaying(jogo->gerenciador_audio.musica_principal) && 
        jogo->status != PARTIDA_FINALIZADA && !jogo->bug_system.em_acao) {
        PlaySound(jogo->gerenciador_audio.musica_principal);
    }
    
    if (jogo->gerenciador_audio.audio_ativo && (jogo->status == PARTIDA_FINALIZADA || jogo->bug_system.em_acao)) {
        StopSound(jogo->gerenciador_audio.musica_principal);
        jogo->gerenciador_audio.audio_ativo = false;
    }
}

// Renderização das Cartas
static void RenderizarCartas(const JogoMemoria *jogo)
{
    int i;
    for(i = 0; i < TOTAL_CARTAS; i++) {
        const CartaJogo *carta = jogo->conjunto_cartas[i];
        
        if (carta->status == CARTA_VIRADA) {
            DrawRectangleRec(carta->bounds, CARTA_VERSO);
        }
        else if (carta->status == CARTA_REVELADA) {
            DrawTexture(carta->textura, carta->bounds.x, carta->bounds.y, WHITE);
        }
        else if (carta->status == CARTA_COMBINADA) {
            DrawRectangleRec(carta->bounds, COR_CARTA_COMBINADA);
            DrawTexture(carta->textura, carta->bounds.x, carta->bounds.y, ColorAlpha(WHITE, TRANSPARENCIA_ENCONTRADA));
        }
        DrawRectangleLinesEx(carta->bounds, LARGURA_BORDA, BORDA_CARTA);
    }
}

// Interface de Usuário

static void RenderizarInterface(const JogoMemoria *jogo)
{
    char texto_p1[100];
    char texto_p2[100];
    snprintf(texto_p1, sizeof(texto_p1), "%s: %d", 
             jogo->participantes[0].nome, jogo->participantes[0].pontos);
    snprintf(texto_p2, sizeof(texto_p2), "%s: %d", 
             jogo->participantes[1].nome, jogo->participantes[1].pontos);
    
    DrawText(texto_p1, 10, 10, 20, jogo->participantes[0].cor_display);
    DrawText(texto_p2, TELA_LARGURA - MeasureText(texto_p2, 20) - 10, 10, 20, jogo->participantes[1].cor_display);
    
    char indicador_turno[100];
    snprintf(indicador_turno, sizeof(indicador_turno), "Turno: %s", jogo->participantes[jogo->turno_atual].nome);
    DrawText(indicador_turno, TELA_LARGURA/2 - MeasureText(indicador_turno, 25)/2, 10, 25, BLACK);
}

// Efeito Visual do Bug
static void RenderizarEfeitoBug(const JogoMemoria *jogo)
{
    if (!jogo->bug_system.em_acao || jogo->bug_system.timer <= 0.0f) return;
    
    float intensidade = jogo->bug_system.timer / DURACAO_EFEITO_BUG;
    const char* msg_bug = "DEU BUG! você perdeu a vez";
    int largura_msg = MeasureText(msg_bug, 40);
    int pos_x = TELA_LARGURA/2 - largura_msg/2;
    int pos_y = TELA_ALTURA/2 - 20;
    
    DrawRectangle(pos_x - 20, pos_y - 10, largura_msg + 40, 60, ColorAlpha(FUNDO_BUG, intensidade * TRANSPARENCIA_BUG));
    DrawText(msg_bug, pos_x, pos_y, 40, ColorAlpha(TEXTO_BUG, intensidade));
}

// Tela de Pausa
static void RenderizarTelaPausa(const JogoMemoria *jogo)
{
    RenderizarCartas(jogo);
    DrawRectangle(0, 0, TELA_LARGURA, TELA_ALTURA, ColorAlpha(WHITE, TRANSPARENCIA_PAUSE));
    
    const char* msg_pausa = "O jogo está pausado, pressione ENTER para retornar!";
    int largura_msg = MeasureText(msg_pausa, 20);
    int pos_x = TELA_LARGURA/2 - largura_msg/2;
    int pos_y = TELA_ALTURA/2 - 10;
    
    DrawRectangle(pos_x - 10, pos_y - 5, largura_msg + 20, 30, ColorAlpha(WHITE, TRANSPARENCIA_BUG));
    DrawText(msg_pausa, pos_x, pos_y, 20, BLACK);
}

// Tela de Vitória
static void RenderizarTelaFinal(const JogoMemoria *jogo)
{
    const char* resultado;
    if (jogo->participantes[0].pontos > jogo->participantes[1].pontos) {
        resultado = jogo->participantes[0].nome;
    } else if (jogo->participantes[1].pontos > jogo->participantes[0].pontos) {
        resultado = jogo->participantes[1].nome;
    } else {
        resultado = "Empate!";
    }
    
    DrawText("FIM DE JOGO!", TELA_LARGURA/2 - MeasureText("FIM DE JOGO!", 50)/2, TELA_ALTURA/2 - 120, 50, BLACK);
    
    char msg_resultado[100];
    snprintf(msg_resultado, sizeof(msg_resultado), "Vencedor: %s", resultado);
    DrawText(msg_resultado, TELA_LARGURA/2 - MeasureText(msg_resultado, 30)/2, TELA_ALTURA/2 - 60, 30, BLACK);
    
    char score1[100], score2[100];
    snprintf(score1, sizeof(score1), "%s: %d", jogo->participantes[0].nome, jogo->participantes[0].pontos);
    snprintf(score2, sizeof(score2), "%s: %d", jogo->participantes[1].nome, jogo->participantes[1].pontos);
    
    DrawText(score1, TELA_LARGURA/2 - MeasureText(score1, 25)/2, TELA_ALTURA/2, 25, jogo->participantes[0].cor_display);
    DrawText(score2, TELA_LARGURA/2 - MeasureText(score2, 25)/2, TELA_ALTURA/2 + 40, 25, jogo->participantes[1].cor_display);
    
    DrawText("Pressione 'R' para jogar novamente", TELA_LARGURA/2 - MeasureText("Pressione 'R' para jogar novamente", 20)/2, TELA_ALTURA/2 + 100, 20, BLACK);
}

//audio
void ConfigurarRecursos(AudioManager *audio)
{
    audio->musica_principal = LoadSound("Memoria/src/music/Fundo.mp3");
    audio->som_acerto = LoadSound("Memoria/src/music/Sucesso.mp3");
    audio->som_erro = LoadSound("Memoria/src/music/Erro.mp3");
    audio->som_carta = LoadSound("Memoria/src/music/Carta.mp3");
    audio->som_termino = LoadSound("Memoria/src/music/Final_Jogo.mp3");
    audio->som_bug = LoadSound("Memoria/src/music/Bug.mp3");
    audio->audio_ativo = false;
}

void LimparRecursos(AudioManager *audio)
{
    UnloadSound(audio->musica_principal);
    UnloadSound(audio->som_acerto);
    UnloadSound(audio->som_erro);
    UnloadSound(audio->som_carta);
    UnloadSound(audio->som_termino);
    UnloadSound(audio->som_bug);
}

void PrepararJogo(JogoMemoria *jogo)
{
    strcpy(jogo->participantes[0].nome, "Dudu");
    jogo->participantes[0].pontos = 0;
    jogo->participantes[0].cor_display = COR_PLAYER1;
    
    strcpy(jogo->participantes[1].nome, "Léo");
    jogo->participantes[1].pontos = 0;
    jogo->participantes[1].cor_display = COR_PLAYER2;
    
    jogo->conjunto_cartas = NULL;
    ResetarEstadoJogo(jogo);
}

void ProcessarJogo(JogoMemoria *jogo)
{
    GerenciarAudio(jogo);
    
    if (jogo->status == PARTIDA_ATIVA) {
        ProcessarSistemaBug(jogo);
        ExecutarComparacaoCartas(jogo);
        
        if (!jogo->bug_system.em_acao) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                TratarCliqueUsuario(jogo);
            }
        }
        
        if (IsKeyPressed(KEY_ENTER) && !jogo->bug_system.em_acao) {
            jogo->status = PARTIDA_PAUSADA;
        }
        
        if (jogo->combinacoes_feitas == PARES_TOTAL) {
            jogo->status = PARTIDA_FINALIZADA;
            PlaySound(jogo->gerenciador_audio.som_termino);
            
            if (jogo->participantes[0].pontos > jogo->participantes[1].pontos) {
                jogo->vencedor_final = 0;
            } else if (jogo->participantes[1].pontos > jogo->participantes[0].pontos) {
                jogo->vencedor_final = 1;
            } else {
                jogo->vencedor_final = -1;
            }
        }
    }
    else if (jogo->status == PARTIDA_PAUSADA) {
        if (IsKeyPressed(KEY_ENTER)) {
            jogo->status = PARTIDA_ATIVA;
        }
    }
    else if (jogo->status == PARTIDA_FINALIZADA) {
        if (IsKeyPressed(KEY_R)) {
            ResetarEstadoJogo(jogo);
        }
    }
}

void RenderizarJogo(const JogoMemoria *jogo)
{
    ClearBackground(FUNDO_TELA);
    
    switch (jogo->status) {
        case PARTIDA_ATIVA:
            RenderizarCartas(jogo);
            RenderizarInterface(jogo);
            RenderizarEfeitoBug(jogo);
            break;
            
        case PARTIDA_PAUSADA:
            RenderizarTelaPausa(jogo);
            break;
            
        case PARTIDA_FINALIZADA:
            RenderizarTelaFinal(jogo);
            break;
    }
}

int gameMemoria(void)
{
    InitWindow(TELA_LARGURA, TELA_ALTURA, "Jogo da Memória");
    InitAudioDevice();
    SetTargetFPS(60);
    srand(time(NULL));
    
    JogoMemoria jogo = {0};
    
    ConfigurarRecursos(&jogo.gerenciador_audio);
    PrepararJogo(&jogo);
    
    while (!WindowShouldClose()) {
        ProcessarJogo(&jogo);
        
        BeginDrawing();
            RenderizarJogo(&jogo);
        EndDrawing();
        
        if (jogo.status == PARTIDA_FINALIZADA && jogo.vencedor_final != -1) {
            WaitTime(2.0);
            
            int resultado = jogo.vencedor_final + 1;
            
            LiberarCartasJogo(jogo.conjunto_cartas);
            LimparRecursos(&jogo.gerenciador_audio);
            CloseAudioDevice();
            CloseWindow();
            
            return resultado;
        }
    }
    
    LiberarCartasJogo(jogo.conjunto_cartas);
    LimparRecursos(&jogo.gerenciador_audio);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}