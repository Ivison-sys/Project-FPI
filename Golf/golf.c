#include "gameGolf.h"
#include <raymath.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//----------------------------------------------------------------------------------
// CONSTANTES
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

//----------------------------------------------------------------------------------
// CONFIGURAÇÃO DO JOGO
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// FUNÇÕES PRIVADAS
//----------------------------------------------------------------------------------
static void CarregarPlataformas(Plataforma plataformas[NUM_PLATAFORMAS], const GameConfig *config)
{
    memset(plataformas, 0, sizeof(Plataforma) * NUM_PLATAFORMAS);
    float centro_x_plat = config->largura_tela / 2.0f;
    float largura_pilar = 100.0f;
    float altura_pilar = 120.0f;
    float largura_total = 400.0f;

    // --- Plataforma 0: Muro sólido ---
    plataformas[0].num_partes_colisao = 1;
    plataformas[0].partes_colisao[0] = (Rectangle){ centro_x_plat - 110, 0, 220, config->altura_tela / 2.0f };
    plataformas[0].deslocamento_y = config->altura_tela / 2.0f;
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
    plataformas[2].partes_colisao[0] = (Rectangle){ centro_x_plat - 25, 0 , 50, config->altura_tela/2 };
    plataformas[2].area_buraco = (Rectangle){ centro_x_plat - 10, 0, 20, 20 };
    plataformas[2].deslocamento_y = config->altura_tela + 150;
}

static void ReiniciarJogo(Game *game)
{
    for (int i = 0; i < CONTAGEM_JOGADORES; i++) {
        game->jogadores[i] = (Bola){
            game->posicoes_iniciais[i],
            game->posicoes_iniciais[i],
            (Vector2){0,0},
            false,
            game->config.cor_jogador[i],
            RAIO_BOLA
        };
        game->lancadores[i] = (Lancador){0, 0, 1, 1, NAO_MIRANDO};
        game->pontuacoes[i] = 0;
    }
    game->jogador_atual = 0;
    game->estadoAtual = JOGANDO;
    game->vencedor = -1;
    
    CarregarPlataformas(game->plataformas, &game->config);
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

static void DesenharTelaVitoria(int vencedor, const GameConfig *config)
{
    DrawRectangle(0, 0, config->largura_tela, config->altura_tela, Fade(BLACK, 0.7f));
    char textoVitoria[100];
    sprintf(textoVitoria, "JOGADOR %d VENCEU!", vencedor + 1);
    int larguraTextoVitoria = MeasureText(textoVitoria, 60);
    DrawText(textoVitoria, config->largura_tela / 2 - larguraTextoVitoria / 2, config->altura_tela / 2 - 60, 60, GOLD);
    const char* textoReiniciar = "Pressione [ENTER] para jogar novamente";
    int larguraTextoReiniciar = MeasureText(textoReiniciar, 20);
    DrawText(textoReiniciar, config->largura_tela / 2 - larguraTextoReiniciar / 2, config->altura_tela / 2 + 20, 20, WHITE);
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
    // Definição da configuração
    game->config = (GameConfig){
        .cor_fundo = (Color){129, 204, 184, 255},
        .cor_jogador = { (Color){0, 121, 241, 255}, (Color){230, 41, 55, 255} },
        .teclas_jogadores = { KEY_A, KEY_L },
        .largura_tela = LARGURA_TELA,
        .altura_tela = ALTURA_TELA
    };

    game->posicoes_iniciais[0] = (Vector2){ 200.0f, game->config.altura_tela / 2.0f };
    game->posicoes_iniciais[1] = (Vector2){ game->config.largura_tela - 200.0f, game->config.altura_tela / 2.0f };
    
    game->agua = (Rectangle){ 200, game->config.altura_tela / 2.0f + 200, game->config.largura_tela - 400, game->config.altura_tela/2 };

    ReiniciarJogo(game);
}
