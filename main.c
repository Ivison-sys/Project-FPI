#include "Memoria/gameMemoria.h"
#include "Golf/gameGolf.h"
#include "Hoquei/gameHoquei.h"

int jogos[3] = {2, 1, 3};

int gerenciarGame(int ordemJogos[3]){
    switch (ordemJogos[0]){
        case 1:
            gameGolf();
        break;
        case 2:
            gameHoquei();
        break;
        case 3: 
            gameMemoria();
        break;
    }

    switch (ordemJogos[1]){
        case 1:
            gameGolf();
        break;
        case 2:
            gameHoquei();
        break;
        case 3: 
            gameMemoria();
        break;
    }

    switch (ordemJogos[2]){
        case 1:
            gameGolf();
        break;
        case 2:
            gameHoquei();
        break;
        case 3: 
            gameMemoria();
        break;
    }

    return 1;
}

int main(){
    // RodarMenu
    // RodarGerador
    gerenciarGame(jogos);
    return 0;
}