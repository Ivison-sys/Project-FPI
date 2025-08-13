#include "Golf/gameGolf.h"
#include "Lib/raymath.h"
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
// MAX_NUVENS já está no .h

// Variáveis estáticas
static Texture2D cenarioGrama;

// Cores
static const Color COR_FUNDO = {129, 204, 184, 255};
static const Color COR_JOGADOR1 = {0, 121, 241, 255};
static const Color COR_JOGADOR2 = {230, 41, 55, 255};
static const Color Marrom = {114, 54, 0, 255};


// Controles
static const int teclas_jogadores[CONTAGEM_JOGADORES] = {KEY_A, KEY_L};

//----------------------------------------------------------------------------------
// FUNÇÕES PARA AS NUVENS
//----------------------------------------------------------------------------------

static void InicializarNuvens(Game *game)
{
    for (int i = 0; i < MAX_NUVENS; i++)
    {
        game->nuvens[i].posicao.x = GetRandomValue(-LARGURA_TELA, LARGURA_TELA);
        game->nuvens[i].posicao.y = GetRandomValue(20, 150);
        game->nuvens[i].raio = GetRandomValue(15, 40);
        game->nuvens[i].velocidade = GetRandomValue(20, 50);
    }
}

static void AtualizarNuvens(Game *game)
{
    float delta = GetFrameTime();
    for (int i = 0; i < MAX_NUVENS; i++)
    {
        game->nuvens[i].posicao.x -= game->nuvens[i].velocidade * delta;

        if (game->nuvens[i].posicao.x < -game->nuvens[i].raio * 4)
        {
            game->nuvens[i].posicao.x = LARGURA_TELA + game->nuvens[i].raio;
            game->nuvens[i].posicao.y = GetRandomValue(20, 150);
            game->nuvens[i].velocidade = GetRandomValue(20, 50);
        }
    }
}

static void DesenharNuvens(const Game *game)
{
    for (int i = 0; i < MAX_NUVENS; i++)
    {
        DrawCircle(game->nuvens[i].posicao.x, game->nuvens[i].posicao.y, game->nuvens[i].raio, Fade(WHITE, 0.9f));
        DrawCircle(game->nuvens[i].posicao.x + game->nuvens[i].raio, game->nuvens[i].posicao.y + 5, game->nuvens[i].raio * 0.8f, Fade(WHITE, 0.8f));
        DrawCircle(game->nuvens[i].posicao.x - game->nuvens[i].raio, game->nuvens[i].posicao.y + 2, game->nuvens[i].raio * 0.7f, Fade(WHITE, 0.8f));
    }
}

//----------------------------------------------------------------------------------
// FUNÇÕES PRINCIPAIS DO JOGO (estáticas)
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

    // --- Plataforma 1: Ladeiras e buraco central 
    plataformas[1].num_partes_colisao = 2;
    plataformas[1].partes_colisao[0] = (Rectangle){ centro_x_plat - largura_total/2, -100, largura_pilar, altura_pilar };
    plataformas[1].partes_colisao[1] = (Rectangle){ centro_x_plat + largura_total/2 - largura_pilar, -100, largura_pilar, altura_pilar };
    plataformas[1].area_buraco = (Rectangle){ centro_x_plat - 50, -20, 100, 40 };
    plataformas[1].num_linhas_colisao = 2;
    plataformas[1].linhas_colisao[0][0] = (Vector2){ centro_x_plat - largura_total/2 + largura_pilar, -100 };
    plataformas[1].linhas_colisao[0][1] = (Vector2){ centro_x_plat - 50, -20 };
    plataformas[1].linhas_colisao[1][0] = (Vector2){ centro_x_plat + 50, -20 };
    plataformas[1].linhas_colisao[1][1] = (Vector2){ centro_x_plat + largura_total/2 - largura_pilar, -100 };

    // --- Plataforma 2: Buraco pequeno 
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
    game->estadoAtual = JOGANDO_G;
    game->vencedor = -1;
    
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
    assets->som_tiro = LoadSound("Golf/sons/taco.wav");
    assets->som_ponto = LoadSound("Golf/sons/queda_buraco.wav");
    assets->som_agua = LoadSound("Golf/sons/water.wav");
    cenarioGrama = LoadTexture("Golf/piso_gramado.png");
}

void DescarregarRecursos(GameAssets *assets)
{
    UnloadSound(assets->som_tiro);
    UnloadSound(assets->som_ponto);
    UnloadSound(assets->som_agua);
    UnloadTexture(cenarioGrama);
}

void InicializarJogo(Game *game)
{
    game->posicoes_iniciais[0] = (Vector2){ 200.0f-5, (ALTURA_TELA / 2.0f)-5 };
    game->posicoes_iniciais[1] = (Vector2){ LARGURA_TELA - 195.0f, (ALTURA_TELA / 2.0f)-5 };
    
    game->agua = (Rectangle){ 200, ALTURA_TELA / 2.0f + 200, LARGURA_TELA - 400, ALTURA_TELA/2 };
    
    InicializarNuvens(game); 
    ReiniciarJogo(game);
}

void AtualizarJogo(Game *game, GameAssets *assets)
{
    AtualizarNuvens(game);

    float bola_parada = 3.0;
    if (game->estadoAtual == JOGANDO_G)
    {
        float delta = GetFrameTime();
        
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
        
        Plataforma *plataforma_ativa = &game->plataformas[game->indice_plataforma_atual];

        for (int pj = 0; pj < CONTAGEM_JOGADORES; pj++) {
            Bola *bola = &game->jogadores[pj];
            Lancador *lancador = &game->lancadores[pj];

            if (!bola->lancada) {
                switch(lancador->estado)
                {
                    case NAO_MIRANDO:
                        if (IsKeyPressed(teclas_jogadores[pj])) {
                            lancador->estado = MIRANDO_ANGULO;
                            if (pj == 0) {
                                lancador->angulo = 0.0f; lancador->direcao_angulo = -1;
                            } else {
                                lancador->angulo = -180.0f; lancador->direcao_angulo = 1;
                            }
                        }
                        break;
                    case MIRANDO_ANGULO:
                        lancador->angulo += VELOCIDADE_ANGULO_LANCAMENTO * lancador->direcao_angulo * delta;
                        if (pj == 0) {
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
                        if (IsKeyPressed(teclas_jogadores[pj])) {
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
                        if (IsKeyPressed(teclas_jogadores[pj])) {
                            bola->lancada = true;
                            float angulo_rad = lancador->angulo * DEG2RAD;
                            float vel_inicial = lancador->potencia * POTENCIA_LANCAMENTO;
                            bola->velocidade = (Vector2){ cosf(angulo_rad) * vel_inicial, sinf(angulo_rad) * vel_inicial };
                            PlaySound(assets->som_tiro);
                            lancador->estado = NAO_MIRANDO;
                        }
                        break;
                }
            } else { 
                bola->posicao_anterior = bola->posicao;
                bola->velocidade.y += GRAVIDADE * delta;
                bola->posicao.x += bola->velocidade.x * 1.8f * delta;
                bola->posicao.y += bola->velocidade.y * 1.8f * delta;
                
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

                bool resetar_bola = false;
                Rectangle gatilho_buraco = plataforma_ativa->area_buraco;
                gatilho_buraco.y += plataforma_ativa->deslocamento_y;

                if (CheckCollisionCircleRec(bola->posicao, bola->raio, gatilho_buraco)) {
                    game->pontuacoes[pj]++;
                    PlaySound(assets->som_ponto);
                    resetar_bola = true;
                    if (game->pontuacoes[pj] >= PONTUACAO_PARA_VENCER) {
                        game->vencedor = pj;
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
                
                if (resetar_bola && game->estadoAtual == JOGANDO_G) {
                    bola->lancada = false;
                    bola->posicao = game->posicoes_iniciais[pj];
                    bola->velocidade = (Vector2){0, 0};
                }
            }
        }
        
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
    DesenharNuvens(game);
    
    DrawRectangle(0, ALTURA_TELA / 2, 200, ALTURA_TELA / 2, Marrom);
    DrawRectangle(LARGURA_TELA - 200, ALTURA_TELA / 2, 200, ALTURA_TELA / 2, Marrom);
    DrawRectangleRec(game->agua, (Color){0, 100, 255, 180});

    DrawTexture(cenarioGrama, LARGURA_TELA - 200, 470, WHITE);
    DrawTexture(cenarioGrama, 0, 470, WHITE);
    
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
    for (int i = 0; i < MAX_ONDULACOES; i++) {
        if (game->ondulacoes[i].ativa) DrawCircleLines(game->ondulacoes[i].centro.x, game->ondulacoes[i].centro.y, game->ondulacoes[i].raio, Fade((Color){0,120,255,255}, game->ondulacoes[i].alpha));
    }
    for (int i = 0; i < CONTAGEM_JOGADORES; i++) DrawCircleV(game->jogadores[i].posicao, game->jogadores[i].raio, game->jogadores[i].cor);

    for (int i = 0; i < CONTAGEM_JOGADORES; i++) {
        const Lancador *l = &game->lancadores[i];
        const Bola *b = &game->jogadores[i];
        if (!b->lancada && l->estado != NAO_MIRANDO) {
            float angulo_rad = l->angulo * DEG2RAD;
            Vector2 ponto_final = { b->posicao.x + cosf(angulo_rad) * 80, b->posicao.y + sinf(angulo_rad) * 80 };
            DrawLineEx(b->posicao, ponto_final, 3.0f, (i==0) ? COR_JOGADOR1 : COR_JOGADOR2);
            
            if (l->estado == AJUSTANDO_POTENCIA) {
                float barraX = (i == 0) ? 10.0f : LARGURA_TELA - 210.0f; // Posição X da barra
                DrawRectangle(barraX, 80, 200, 20, LIGHTGRAY);
                DrawRectangle(barraX, 80, (int)(l->potencia * 2.0f), 20, RED);
                DrawRectangleLines(barraX, 80, 200, 20, DARKGRAY);
            }
        }
    }
    
    char texto_pontuacao[64];
    sprintf(texto_pontuacao, "DUDU: %d | LÉO: %d", game->pontuacoes[0], game->pontuacoes[1]);
    DrawText(texto_pontuacao, LARGURA_TELA / 2 - MeasureText(texto_pontuacao, 30) / 2, 10, 30, BLACK);
    
    for (int i = 0; i < CONTAGEM_JOGADORES; i++) {
        const Lancador *l = &game->lancadores[i];
        const char *instrucao = "";
        switch(l->estado) {
            case NAO_MIRANDO:        instrucao = "Pressione para mirar"; break;
            case MIRANDO_ANGULO:     instrucao = "Pressione para travar o angulo"; break;
            case AJUSTANDO_POTENCIA: instrucao = "Pressione para lancar"; break;
        }
        char linha[128];
        sprintf(linha, "Jogador %d ('%c'): %s", i+1, (i==0)?'A':'L', instrucao);
        if (i == 0) DrawText(linha, 10, 40, 20, DARKGRAY);
        else DrawText(linha, LARGURA_TELA - MeasureText(linha,20) - 10, 40, 20, DARKGRAY);
    }
    
    DrawRectangle(LARGURA_TELA / 2 - MeasureText(texto_pontuacao, 30) / 2 - 10, 8, MeasureText("JOGADOR 1: 0", 30) + 15, 34, Fade(COR_JOGADOR1, 0.15f));
    DrawRectangle(LARGURA_TELA / 2 + MeasureText("JOGADOR 1: 0 | ", 30) - 15, 8, MeasureText("JOGADOR 2: 0", 30) + 15, 34, Fade(COR_JOGADOR2, 0.15f));
    
    if (game->estadoAtual == FIM_DE_JOGO) {
        DesenharTelaVitoria(game->vencedor);
    }
}

// --- Implementação da função pública ---
// A assinatura foi alterada para `int gameGolf(void)`
int gameGolf(void)
{
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Jogo de Golfe");
    InitAudioDevice();
    SetTargetFPS(90);

    Game game = {0};
    GameAssets assets = {0};

    CarregarRecursos(&assets);
    InicializarJogo(&game);

    while (!WindowShouldClose())
    {
        AtualizarJogo(&game, &assets);

        BeginDrawing();
            DesenharJogo(&game);
        EndDrawing();
        
        // Verifica se o jogo terminou e se há um vencedor
        if (game.estadoAtual == FIM_DE_JOGO && game.vencedor != -1)
        {

            DescarregarRecursos(&assets);
            CloseAudioDevice();
            CloseWindow();
            return game.vencedor + 1;
        }
    }

    // Se o loop terminar porque a janela foi fechada, limpa tudo e retorna 0
    DescarregarRecursos(&assets);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}