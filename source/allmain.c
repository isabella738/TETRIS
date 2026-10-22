//make && ./TETRIS
#include "settings.h"
#include "sprites.h"
#include "functions.h"
#include "terminal.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <stdlib.h>

#if defined(LINUX)
    #include <fcntl.h>
    #include <unistd.h>
#endif

int main(){
    limpar_tela();    
    srand(time(NULL));
    #if defined(LINUX)
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
        mudar_terminal();
        system("clear");
    #else
        system("cls");
    #endif
    
    for(int i=0; i<7; i++) printf("%s\n", inicio[i]);
    pausa(); limpar_tela();
    

    iniciar_mapa();

    spawn(); spawn();
    int cd_descer = TEMPO_DESCIDA, frames=0;

    do{
        frames++;
        printf("%d\n", frames);
        cd_descer--;
        if(!cd_descer){
            movimentar(2);
            cd_descer = TEMPO_DESCIDA;
        }
        verificar_linhas();

        comandos();
        
        imprimir();
        info_para_debug();

        if(derrota()) break;
        
        usleep(50000);
        printf(VOLTAR);
    }while(1);
    printf("\nBom Jogo\n");
    
    restaurar_terminal();

    return 0;
}