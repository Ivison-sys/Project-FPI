#ifndef GAME_H
#define GAME_H

#include <raylib.h>

//----------------------------------------------------------------------------------
// CONFIGURAÇÕES DO JOGO (Constantes Públicas)
//----------------------------------------------------------------------------------
#define LARGURA_TELA 1680
#define ALTURA_TELA 940
#define CONTAGEM_JOGADORES 2
#define PONTUACAO_PARA_VENCER 5
#define MAX_NUVENS 10

//----------------------------------------------------------------------------------
// ESTRUTURAS DE DADOS
//----------------------------------------------------------------------------------

typedef struct Nuvem {
    Vector2 posicao;
    float raio;
    float velocidade;
} Nuvem;

typedef enum GameState_G {
    JOGANDO_G,
    FIM_DE_JOGO
} GameState;

typedef enum EstadoLancamento {
    NAO_MIRANDO,
    MIRANDO_ANGULO,
    AJUSTANDO_POTENCIA
} EstadoLancamento;

typedef struct Bola {
    Vector2 posicao;
    Vector2 posicao_anterior;
    Vector2 velocidade;
    bool lancada;
    Color cor;
    float raio;
} Bola;

typedef struct Lancador {
    float angulo;
    float potencia;
    int direcao_potencia;
    int direcao_angulo;
    EstadoLancamento estado;
} Lancador;

typedef struct Ondulacao {
    Vector2 centro;
    float raio;
    float alpha;
    bool ativa;
} Ondulacao;

typedef struct Plataforma {
    Rectangle partes_colisao[5]; // MAX_PARTES_COLISAO
    Vector2 linhas_colisao[5][2]; // MAX_LINHAS_COLISAO
    int num_partes_colisao;
    int num_linhas_colisao;
    float deslocamento_y;
    Rectangle area_buraco;
    Rectangle area_buraco_visual;
} Plataforma;

// Estrutura para guardar os recursos (sons) do jogo
typedef struct GameAssets {
    Sound som_tiro;
    Sound som_ponto;
    Sound som_agua;
} GameAssets;

// Estrutura principal que contém TODO o estado do jogo
typedef struct Game {
    GameState estadoAtual;
    int vencedor;

    Bola jogadores[CONTAGEM_JOGADORES];
    Lancador lancadores[CONTAGEM_JOGADORES];
    Vector2 posicoes_iniciais[CONTAGEM_JOGADORES];
    int pontuacoes[CONTAGEM_JOGADORES];
    int jogador_atual; // Mantido para compatibilidade, mas não usado na lógica de turnos
    
    Plataforma plataformas[3]; // NUM_PLATAFORMAS
    int indice_plataforma_atual;
    bool plataforma_esta_animando;
    float progresso_animacao_plataforma;

    Ondulacao ondulacoes[10]; // MAX_ONDULACOES
    Rectangle agua;

    // Array para as nuvens
    Nuvem nuvens[MAX_NUVENS];

} Game;


//----------------------------------------------------------------------------------
// ASSINATURAS DAS FUNÇÕES PÚBLICAS
//----------------------------------------------------------------------------------
void CarregarRecursos(GameAssets *assets);
void DescarregarRecursos(GameAssets *assets);

void InicializarJogo(Game *game);
void AtualizarJogo(Game *game, GameAssets *assets);
void DesenharJogo(const Game *game); 

// A função retorna o número do vencedor (1 ou 2) ou 0 se a janela for fechada.
int gameGolf(void); 

#endif // GAME_H
