#ifndef SORTEIO_H
#define SORTEIO_H

#include <stddef.h> // inclui pra usar size_t

// ids fixos pros 3 minijogos (1 = golf, 2 = hoquei, 3 = memoria)
typedef enum {
    JOGO_GOLF   = 1,
    JOGO_HOQUEI = 2,
    JOGO_MEMORIA= 3
} JogoId;

// funções dos minijogos (trocar nomes se no seu projeto forem diferentes)
void gameGolf(void);
void gameHoquei(void);
void gameMemoria(void);

// embaralha um vetor de inteiros mudando a ordem dos elementos aleatoriamente
void embaralhar_ints(int *v, size_t n);

// sorteia uma ordem de ids (1..3) e coloca no array passado
void sorteia_ordem_ids(int ordem[3]);

// sorteia uma ordem mas já retorna ponteiros pras funções dos jogos
void sorteia_ordem_funcoes(void (*ordem[3])(void));

// mostra uma telinha animada indicando a ordem sorteada (pra usar os prints aqui)
void tela_sorteio_preview(const int ordem[3]);

#endif
