#ifndef GAME_MEMORIA_H
#define GAME_MEMORIA_H

#include "../Lib/raylib.h"
#include <stdbool.h>

// Configurações do Jogo
#define TELA_LARGURA 800
#define TELA_ALTURA 600
#define TOTAL_CARTAS 50
#define TAMANHO_CARTA 60
#define ESPACAMENTO_CARTA 6
#define NOME_MAX 50
#define PARES_TOTAL 24
#define DURACAO_COMPARACAO 1.0f
#define DURACAO_EFEITO_BUG 7.0f

// Estados 
typedef enum {
    CARTA_VIRADA,
    CARTA_REVELADA,
    CARTA_COMBINADA
} StatusCarta;

typedef enum {
    PARTIDA_ATIVA,
    PARTIDA_PAUSADA,
    PARTIDA_FINALIZADA
} StatusJogo;

// Estruturas de Dados
typedef struct {
    StatusCarta status;
    Rectangle bounds;
    Texture2D textura;
    char* identificador;
    bool bug_ativado;
} CartaJogo;

typedef struct {
    char nome[NOME_MAX];
    int pontos;
    Color cor_display;
} Participante;

typedef struct {
    bool em_acao;
    float timer;
} SistemaBug;

typedef struct {
    Sound musica_principal;
    Sound som_acerto;
    Sound som_erro;
    Sound som_carta;
    Sound som_termino;
    Sound som_bug;
    bool audio_ativo;
} AudioManager;

typedef struct {
    CartaJogo* primeira;
    CartaJogo* segunda;
    bool processando;
    float cronometro;
} ComparadorCartas;

// Estrutura Principal do Jogo
typedef struct {
    StatusJogo status;
    int turno_atual;
    int vencedor_final;
    int combinacoes_feitas;

    Participante participantes[2];
    CartaJogo** conjunto_cartas;
    
    ComparadorCartas comparador;
    SistemaBug bug_system;
    AudioManager gerenciador_audio;

} JogoMemoria;


//
void ConfigurarRecursos(AudioManager *audio);
void LimparRecursos(AudioManager *audio);

void PrepararJogo(JogoMemoria *jogo);
void ProcessarJogo(JogoMemoria *jogo);
void RenderizarJogo(const JogoMemoria *jogo);

int gameMemoria(void);

#endif