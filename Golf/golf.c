// golf.c
#include "golf.h"
#include <raymath.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//----------------------------------------------------------------------------------
// CONSTANTES INTERNAS AO MÓDULO
//----------------------------------------------------------------------------------
#define GRAVIDADE 400.0f
#define ATRITO 0.98f
#define RAIO_BOLA 8.0f
#define VELOCIDADE_ANGULO_LANCAMENTO 90.0f
#define VELOCIDADE_LANCAMENTO 120.0f
#define POTENCIA_LANCAMENTO 10.0f
#define MAX_ONDULACOES 10
#define NUM_PLATAFORMAS 3
#define VELOCIDADE_ANIMACAO 1.5f
#define MAX_COLISAO 5
#define MAX_COLISAO 5

// Cores
static const Color COR_FUNDO = {129, 204, 184, 255};
static const Color COR_JOGADOR1 = {0, 121, 241, 255};
static const Color COR_JOGADOR2 = {230, 41, 55, 255};

// Controles
static const int teclas_jogadores[CONTAGEM_JOGADORES] = {KEY_A, KEY_L};

//----------------------------------------------------------------------------------
// FUNÇÕES PRIVADAS (estáticas)
//----------------------------------------------------------------------------------
static void CarregarPlataformas(Plataforma plataformas[NUM_PLATAFORMAS])
{
    memset(plataformas, 0, sizeof(Plataforma) * NUM_PLATAFORMAS);
    float centro_x_plat = LARGURA_TELA / 2.0f;
    float largura_pilar = 100.0f;
    float altura_pilar = 120.0f;
    float largura_total = 400.0f;

    // --- Plataforma 0: Muro sólido ---
    plataformas[0].num_partes_colisao = 1;
    plataformas[0].partes_colisao[0] = (Rectangle){ centro_x_plat - 110, 0, 220, ALTURA_TELA / 2.0f };
    plataformas[0].deslocamento_y = ALTURA_TELA / 2.0f;
    plataformas[0].area_buraco = (Rectangle){ centro_x_plat - 25, 0, 50, 50 };

    // --- Plataforma 1: Ladeiras e buraco central ---
    plataformas[1].num_partes_colisao = 2;
    plataformas[1].partes_colisao[0] = (Rectangle){ centro_x_plat - largura_total/2, -100, largura_pilar, altura_pilar };
    plataformas[1].partes_colisao[1] = (Rectangle){ centro_x_plat + largura_total/2 - largura_pilar, -100, largura_pilar, altura_pilar };
    plataformas[1].area_buraco = (Rectangle){ centro_x_plat - 50, -20, 100, 40 };
    plataformas[1].num_linhas_colisao = 2;
    plataformas[1].linhas_colisao[0][0] = (Vector2){ centro_x_plat - largura_total/2 + largura_pilar, -100 };
    plataformas[1].linhas_colisao[0][1] = (Vector2){ centro_x_plat - 50, -20 };
    plataformas[1].linhas_colisao[1][0] = (Vector2){ centro_x_plat + 50, -20 };
    plataformas[1].linhas_colisao[1][1] = (Vector2){ centro_x_plat + largura_total/2 - largura_pilar, -100 };

    // --- Plataforma 2: Buraco pequeno ---
    plataformas[2].num_partes_colisao = 1;
    plataformas[2].partes_colisao[0] = (Rectangle){ centro_x_plat - 25, 0 , 50, ALTURA_TELA/2 };
    plataformas[2].area_buraco = (Rectangle){ centro_x_plat - 10, 0, 20, 20 };
    plataformas[2].deslocamento_y = ALTURA_TELA + 150;
}

static void ReiniciarJogo(Game *game)
{
    for (int i = 0; i < CONTAGEM_JOGADORES; i++) {
        game->jogadores[i] = (Bola){game->posicoes_iniciais[i], game->posicoes_iniciais[i], {0,0}, false, (i==0 ? COR_JOGADOR1 : COR_JOGADOR2), RAIO_BOLA};
        game->lancadores[i] = (Lancador){0, 0, 1, 1, NAO_MIRANDO};
        game->pontuacoes[i] = 0;
    }
    game->jogador_atual = 0;
    game->estadoAtual = JOGANDO;
    game->vencedor = -1;
    
    // Resetar posições das plataformas se necessário
    CarregarPlataformas(game->plataformas);
    game->indice_plataforma_atual = 0;
    game->plataforma_esta_animando = false;
    game->progresso_animacao_plataforma = 0.0f;
}

static void ResolverColisaoBolaLinha(Bola *bola, Vector2 p1, Vector2 p2)
{
    Vector2 linhaVetor = Vector2Subtract(p2, p1);
    Vector2 vetorParaBola = Vector2Subtract(bola->posicao, p1);
    float t = Vector2DotProduct(vetorParaBola, linhaVetor) / Vector2DotProduct(linhaVetor, linhaVetor);
    t = fmaxf(0, fminf(1, t));
    Vector2 pontoMaisProximo = Vector2Add(p1, Vector2Scale(linhaVetor, t));
    Vector2 vetorColisao = Vector2Subtract(bola->posicao, pontoMaisProximo);
    Vector2 normalColisao = Vector2Normalize(vetorColisao);
    bola->posicao = Vector2Add(pontoMaisProximo, Vector2Scale(normalColisao, bola->raio));
    Vector2 normalLinha = Vector2Normalize((Vector2){-linhaVetor.y, linhaVetor.x});
    float dot = Vector2DotProduct(bola->velocidade, normalLinha);
    Vector2 reflexao = Vector2Scale(normalLinha, (dot / 2) + 2);
    bola->velocidade = Vector2Subtract(bola->velocidade, reflexao);
    bola->velocidade = Vector2Scale(bola->velocidade, ATRITO);
}

static void DesenharTelaVitoria(int vencedor)
{
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, 0.7f));
    char textoVitoria[100];
    sprintf(textoVitoria, "JOGADOR %d VENCEU!", vencedor + 1);
    int larguraTextoVitoria = MeasureText(textoVitoria, 60);
    DrawText(textoVitoria, LARGURA_TELA / 2 - larguraTextoVitoria / 2, ALTURA_TELA / 2 - 60, 60, GOLD);
    const char* textoReiniciar = "Pressione [ENTER] para jogar novamente";
    int larguraTextoReiniciar = MeasureText(textoReiniciar, 20);
    DrawText(textoReiniciar, LARGURA_TELA / 2 - larguraTextoReiniciar / 2, ALTURA_TELA / 2 + 20, 20, WHITE);
}

//----------------------------------------------------------------------------------
// FUNÇÕES PÚBLICAS
//----------------------------------------------------------------------------------

void CarregarRecursos(GameAssets *assets)
{
    assets->som_tiro = LoadSound("resources/taco.wav");
    assets->som_ponto = LoadSound("resources/queda_buraco.wav");
    assets->som_agua = LoadSound("resources/water.wav");
}

void DescarregarRecursos(GameAssets *assets)
{
    UnloadSound(assets->som_tiro);
    UnloadSound(assets->som_ponto);
    UnloadSound(assets->som_agua);
}

void InicializarJogo(Game *game)
{
    game->posicoes_iniciais[0] = (Vector2){ 200.0f, ALTURA_TELA / 2.0f };
    game->posicoes_iniciais[1] = (Vector2){ LARGURA_TELA - 200.0f, ALTURA_TELA / 2.0f };
    
    game->agua = (Rectangle){ 200, ALTURA_TELA / 2.0f + 200, LARGURA_TELA - 400, ALTURA_TELA/2 };

    ReiniciarJogo(game); // ReiniciarJogo contém a lógica para setar o estado inicial
}

void AtualizarJogo(Game *game, GameAssets *assets)
{
    float bola_parada = 3.0;
    if (game->estadoAtual == JOGANDO)
    {
        float delta = GetFrameTime();
        
        // --- Animação da Plataforma ---
        if (game->plataforma_esta_animando) {
            game->progresso_animacao_plataforma += VELOCIDADE_ANIMACAO * delta;
            float y_na_tela = ALTURA_TELA / 2.0f;
            float y_fora_da_tela = ALTURA_TELA + 150.0f;
            int indice_saindo = game->indice_plataforma_atual;
            int indice_entrando = (game->indice_plataforma_atual + 1) % NUM_PLATAFORMAS;
            game->plataformas[indice_saindo].deslocamento_y = Lerp(y_na_tela, y_fora_da_tela, game->progresso_animacao_plataforma);
            game->plataformas[indice_entrando].deslocamento_y = Lerp(y_fora_da_tela, y_na_tela, game->progresso_animacao_plataforma);
            if (game->progresso_animacao_plataforma >= 1.0f) {
                game->progresso_animacao_plataforma = 0.0f; game->plataforma_esta_animando = false;
                game->plataformas[indice_saindo].deslocamento_y = y_fora_da_tela;
                game->plataformas[indice_entrando].deslocamento_y = y_na_tela;
                game->indice_plataforma_atual = indice_entrando;
            }
        }
        
        // --- Lógica do Jogador Atual ---
        Bola *bola = &game->jogadores[game->jogador_atual];
        Lancador *lancador = &game->lancadores[game->jogador_atual];
        
        if (!bola->lancada) {
            switch(lancador->estado)
            {
                case NAO_MIRANDO:
                    if (IsKeyPressed(teclas_jogadores[game->jogador_atual])) {
                        lancador->estado = MIRANDO_ANGULO;
                        if (game->jogador_atual == 0) {
                            lancador->angulo = 0.0f; lancador->direcao_angulo = -1;
                        } else {
                            lancador->angulo = -180.0f; lancador->direcao_angulo = 1;
                        }
                    }
                    break;
                case MIRANDO_ANGULO:
                    lancador->angulo += VELOCIDADE_ANGULO_LANCAMENTO * lancador->direcao_angulo * delta;
                    if (game->jogador_atual == 0) {
                        if (lancador->angulo <= -90.0f || lancador->angulo >= 0.0f) {
                            lancador->direcao_angulo *= -1;
                            lancador->angulo = Clamp(lancador->angulo, -90.0f, 0.0f);
                        }
                    } else {
                        if (lancador->angulo >= -90.0f || lancador->angulo <= -180.0f) {
                            lancador->direcao_angulo *= -1;
                            lancador->angulo = Clamp(lancador->angulo, -180.0f, -90.0f);
                        }
                    }
                    if (IsKeyPressed(teclas_jogadores[game->jogador_atual])) {
                        lancador->estado = AJUSTANDO_POTENCIA;
                        lancador->potencia = 0; lancador->direcao_potencia = 1;
                    }
                    break;
                case AJUSTANDO_POTENCIA:
                    lancador->potencia += VELOCIDADE_LANCAMENTO * lancador->direcao_potencia * delta;
                    if (lancador->potencia >= 100.0f || lancador->potencia <= 0.0f) {
                        lancador->direcao_potencia *= -1;
                        lancador->potencia = Clamp(lancador->potencia, 0.0f, 100.0f);
                    }
                    if (IsKeyPressed(teclas_jogadores[game->jogador_atual])) {
                        bola->lancada = true;
                        float angulo_rad = lancador->angulo * DEG2RAD;
                        float vel_inicial = lancador->potencia * POTENCIA_LANCAMENTO;
                        bola->velocidade = (Vector2){ cosf(angulo_rad) * vel_inicial, sinf(angulo_rad) * vel_inicial };
                        PlaySound(assets->som_tiro);
                        lancador->estado = NAO_MIRANDO;
                    }
                    break;
            }
        } else { // Bola foi lançada
            // --- Física e Colisão ---
            bola->posicao_anterior = bola->posicao;
            bola->velocidade.y += GRAVIDADE * delta;
            bola->posicao.x += bola->velocidade.x * 1.8f * delta;
            bola->posicao.y += bola->velocidade.y * 1.8f * delta;
            
            Plataforma *plataforma_ativa = &game->plataformas[game->indice_plataforma_atual];
            
            for (int i = 0; i < plataforma_ativa->num_partes_colisao; i++) {
                Rectangle parte = plataforma_ativa->partes_colisao[i];
                parte.y += plataforma_ativa->deslocamento_y;
                if (CheckCollisionCircleRec(bola->posicao, bola->raio, parte)) {
                    if (bola->posicao_anterior.y + bola->raio <= parte.y) {
                        bola->velocidade.y = 0; bola->posicao.y = parte.y - bola->raio;
                        bola->velocidade.x *= ATRITO;
                    } else {
                        bola->velocidade.x *= -0.9f;
                        if (bola->posicao.x < parte.x + parte.width / 2) bola->posicao.x = parte.x - bola->raio;
                        else bola->posicao.x = parte.x + parte.width + bola->raio;
                    }
                }
            }
            for (int i = 0; i < plataforma_ativa->num_linhas_colisao; i++) {
                Vector2 p1 = plataforma_ativa->linhas_colisao[i][0];
                Vector2 p2 = plataforma_ativa->linhas_colisao[i][1];
                p1.y += plataforma_ativa->deslocamento_y;
                p2.y += plataforma_ativa->deslocamento_y;
                if (CheckCollisionCircleLine(bola->posicao, bola->raio, p1, p2)) {
                    ResolverColisaoBolaLinha(bola, p1, p2);
                }
            }

            // --- Reset e Pontuação ---
            bool resetar_bola = false;
            Rectangle gatilho_buraco = plataforma_ativa->area_buraco;
            gatilho_buraco.y += plataforma_ativa->deslocamento_y;

            if (CheckCollisionCircleRec(bola->posicao, bola->raio, gatilho_buraco)) {
                game->pontuacoes[game->jogador_atual]++;
                PlaySound(assets->som_ponto);
                resetar_bola = true;
                if (game->pontuacoes[game->jogador_atual] >= PONTUACAO_PARA_VENCER) {
                    game->vencedor = game->jogador_atual;
                    game->estadoAtual = FIM_DE_JOGO;
                } else {
                    if (!game->plataforma_esta_animando) game->plataforma_esta_animando = true;
                }
            }

            if(CheckCollisionCircleRec(bola->posicao, bola->raio, game->agua)) {
                for (int i = 0; i < MAX_ONDULACOES; i++) {
                    if (!game->ondulacoes[i].ativa) {
                        game->ondulacoes[i] = (Ondulacao){ bola->posicao, 5.0f, 1.0f, true };
                        PlaySound(assets->som_agua); break;
                    }
                }
            }

            if (fabsf(bola->velocidade.x) < bola_parada && fabsf(bola->velocidade.y) < bola_parada) resetar_bola = true;
            if (bola->posicao.y > ALTURA_TELA || bola->posicao.x < 0 || bola->posicao.x > LARGURA_TELA) resetar_bola = true;
            
            if (resetar_bola && game->estadoAtual == JOGANDO) {
                bola->lancada = false;
                bola->posicao = game->posicoes_iniciais[game->jogador_atual];
                bola->velocidade = (Vector2){0, 0};
                game->jogador_atual = (game->jogador_atual + 1) % CONTAGEM_JOGADORES;
            }
        }
        
        // --- Ondulações da água ---
        for (int i = 0; i < MAX_ONDULACOES; i++) {
            if (game->ondulacoes[i].ativa) {
                game->ondulacoes[i].raio += 100 * delta; game->ondulacoes[i].alpha -= 0.5f * delta;
                if (game->ondulacoes[i].alpha <= 0) game->ondulacoes[i].ativa = false;
            }
        }
    }
    else if (game->estadoAtual == FIM_DE_JOGO)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            ReiniciarJogo(game);
        }
    }
}

void DesenharJogo(const Game *game)
{
    ClearBackground(COR_FUNDO);
    
    // Cenário
    DrawRectangle(0, ALTURA_TELA / 2, 200, ALTURA_TELA / 2, DARKGRAY);
    DrawRectangle(LARGURA_TELA - 200, ALTURA_TELA / 2, 200, ALTURA_TELA / 2, DARKGRAY);
    DrawRectangleRec(game->agua, (Color){0, 100, 255, 180});

    // Plataformas
    for (int idx_p = 0; idx_p < NUM_PLATAFORMAS; idx_p++) {
        const Plataforma *p = &game->plataformas[idx_p];
        if (p->deslocamento_y > ALTURA_TELA/2 - 150 && p->deslocamento_y < ALTURA_TELA + 150) {
            for (int i = 0; i < p->num_partes_colisao; i++) {
                Rectangle parte = p->partes_colisao[i];
                parte.y += p->deslocamento_y;
                DrawRectangleRec(parte, GRAY);
            }
            for (int i = 0; i < p->num_linhas_colisao; i++) {
                Vector2 p1 = p->linhas_colisao[i][0];
                Vector2 p2 = p->linhas_colisao[i][1];
                p1.y += p->deslocamento_y; p2.y += p->deslocamento_y;
                DrawLineEx(p1, p2, 5.0f, GRAY);
            }
            Rectangle buraco_para_desenhar = p->area_buraco;
            buraco_para_desenhar.y += p->deslocamento_y;
            DrawRectangleRec(buraco_para_desenhar, Fade(BLACK, 0.6f));
        }
    }

    // Efeitos e Jogadores
    for (int i = 0; i < MAX_ONDULACOES; i++) {
        if (game->ondulacoes[i].ativa) DrawCircleLines(game->ondulacoes[i].centro.x, game->ondulacoes[i].centro.y, game->ondulacoes[i].raio, Fade((Color){0,120,255,255}, game->ondulacoes[i].alpha));
    }
    for (int i = 0; i < CONTAGEM_JOGADORES; i++) DrawCircleV(game->jogadores[i].posicao, game->jogadores[i].raio, game->jogadores[i].cor);

    // Mira e Barra de Força
    const Lancador *lancador = &game->lancadores[game->jogador_atual];
    const Bola *bola_atual = &game->jogadores[game->jogador_atual];
    if (!bola_atual->lancada && lancador->estado != NAO_MIRANDO) {
        float angulo_rad = lancador->angulo * DEG2RAD;
        Vector2 ponto_final = { bola_atual->posicao.x + cosf(angulo_rad) * 80, bola_atual->posicao.y + sinf(angulo_rad) * 80 };
        DrawLineEx(bola_atual->posicao, ponto_final, 3.0f, MAROON);
        
        if (lancador->estado == AJUSTANDO_POTENCIA) {
            DrawRectangle(10, 80, 200, 20, LIGHTGRAY);
            DrawRectangle(10, 80, (int)(lancador->potencia * 2.0f), 20, RED);
            DrawRectangleLines(10, 80, 200, 20, DARKGRAY);
        }
    }
    
    // HUD (Interface)
    char texto_pontuacao[64];
    sprintf(texto_pontuacao, "JOGADOR 1: %d | JOGADOR 2: %d", game->pontuacoes[0], game->pontuacoes[1]);
    DrawText(texto_pontuacao, LARGURA_TELA / 2 - MeasureText(texto_pontuacao, 30) / 2, 10, 30, BLACK);
    
    char texto_vez_jogador[128];
    const char* instrucao = "";
    switch(lancador->estado) {
        case NAO_MIRANDO:        instrucao = "Pressione para mirar"; break;
        case MIRANDO_ANGULO:     instrucao = "Pressione para travar o angulo"; break;
        case AJUSTANDO_POTENCIA: instrucao = "Pressione para lancar"; break;
    }
    sprintf(texto_vez_jogador, "Vez do Jogador %d ('%c') - %s", game->jogador_atual + 1, (game->jogador_atual == 0) ? 'A' : 'L', instrucao);
    DrawText(texto_vez_jogador, 10, 10, 20, DARKGRAY);
    
    if (game->jogador_atual == 0) {
        DrawRectangle(LARGURA_TELA / 2 - MeasureText(texto_pontuacao, 30) / 2 - 10, 8, MeasureText("JOGADOR 1: 0", 30) + 15, 34, Fade(COR_JOGADOR1, 0.3f));
    } else {
        DrawRectangle(LARGURA_TELA / 2 - MeasureText(texto_pontuacao, 30) / 2 + MeasureText("JOGADOR 1: 0 | ", 30) - 5, 8, MeasureText("JOGADOR 2: 0", 30) + 15, 34, Fade(COR_JOGADOR2, 0.3f));
    }
    
    if (game->estadoAtual == FIM_DE_JOGO) {
        DesenharTelaVitoria(game->vencedor);
    }
}
// game.c

// ... (todo o código que já estava aqui, como AtualizarJogo, DesenharJogo, etc.)

// Implementação da nova função pública
void GameGolf(void)
{
    // Inicialização da Janela e Áudio
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Jogo de Golfe");
    InitAudioDevice();
    SetTargetFPS(90);

    // Cria as estruturas para o estado e recursos do jogo
    Game game = {0};
    GameAssets assets = {0};

    // Carrega os recursos e inicializa o estado do jogo
    CarregarRecursos(&assets);
    InicializarJogo(&game);

    // Loop Principal do Jogo de Golfe
    while (!WindowShouldClose())
    {
        AtualizarJogo(&game, &assets);

        BeginDrawing();
            DesenharJogo(&game);
        EndDrawing();
    }

    // Descarregamento e Finalização
    DescarregarRecursos(&assets);
    CloseAudioDevice();
    CloseWindow();
}