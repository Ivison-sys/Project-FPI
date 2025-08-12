#include "Memoria/gameMemoria.h"
#include "Golf/gameGolf.h"
#include "Hoquei/gameHoquei.h"
#include "Sorteio/sorteio.h"
#include "Mensagens/mensagens.h"
#include "Menu/gameMenu.h"

int jogos[3] = {1, 2, 3};

int main(){
    // RodarMenu
    // RodarGerador
    if(menu()){
        tela_sorteio_preview(jogos);
        executarMsgs(jogos);
    }
    return 0;
}