#include "settings.h"
#include "auxiliary.h"
#include "sprites.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void iniciar_mapa(){
    for(int y=0; y<ALTURA; y++){
        for(int x=0; x<LARGURA; x++){
            mapa[y][x].cor = 0;

            (Mapa[y][x] == ' ' || Mapa[y][x] == '_') ? (mapa[y][x].tipo = 0) : (mapa[y][x].tipo = 1);
        }
    }
}

void spawn(){
    int b = rand()%MAX_BLOCOS;
    int c = rand()%MAX_CORES + 1;

    bloco = proximo;

    proximo.cor = c;
    memcpy(proximo.uso, uso[b], sizeof(proximo.uso));
    proximo.p = (Coordenada){8, 0};
}

void congelar(){
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            if(bloco.uso[i][j]){
                int x = bloco.p.x + j;
                int y = bloco.p.y + i;
                mapa[y][x].tipo = 2;
                mapa[y][x].cor = bloco.cor;
            }
        }
    }
}

void rotacionar(int sentido){//0 = horario, 1 = antihorario
    int novo_uso[4][4];

    if(sentido) {
        for(int y=0; y<4; y++){
            for(int x=0; x<4; x++){
                novo_uso[y][x] = bloco.uso[x][3-y];
            }
        }
    }
    else{
        for(int y=0; y<4; y++){
            for(int x=0; x<4; x++){
                novo_uso[y][x] = bloco.uso[3-x][y];
            }
        }
    }

    //Este evita que, ao rotacionar, o bloco sobreponha outro ou a propria parede
    //Se for o caso, a rotação é bloqueada
    int continuar=1;
    for(int i=0; i<4 && continuar; i++){
        for(int j=0; j<4 && continuar; j++){
            if(novo_uso[i][j]){
                if(mapa[bloco.p.y + i][bloco.p.x + j].tipo){
                    continuar=0;
                }
            }
        }
    }

    if(continuar){
        memcpy(bloco.uso, novo_uso, sizeof(novo_uso));
    }
}

void movimentar(int d){
    /*
        1 = esquerda (x--)
        2 = cima (y--)
        3 = direita (x++)
        4 = baixo (y++)
    */

    int x = direcao[d].x;
    int y = direcao[d].y;

    //Verifica se ir para a posicao desejada nao sobrepoe algum bloco
    //Caso seja o caso, o movimento é bloqueado
    int continuar=1;
    for(int i=0; i<4 && continuar; i++){
        for(int j=0; j<4 && continuar; j++){
            if(bloco.uso[i][j]){
                if(mapa[bloco.p.y + i + y][bloco.p.x + j + x].tipo){
                    continuar=0;
                }
            }
        }
    }

    if(continuar){
        bloco.p.x += x;
        bloco.p.y += y;
    }
    if(!continuar && d == 2){
        congelar();
        spawn();
    }
}

void deslocar_linha(int i){
    for(int y=i; y>0; y--){
        for(int x=1; x<LARGURA-1; x++){
            mapa[y][x] = mapa[y-1][x];
        }
    }
}

void apagar_linha(int i){
    for(int y=i; y>=0; y--){
        for(int x=1; x<LARGURA-1; x++){
            mapa[y][x].cor = 0;
        }
    }
}

int verificar_linhas(){

    int apagar[ALTURA-1] = {0};
    int apagou=1;

    for(int y=ALTURA-2; y>=0; y--){    
        int continuar=1;

        for(int i=1; i<LARGURA-1 && continuar; i++){
            if(mapa[y][i].tipo == 0){
                continuar = 0;        
            }
        }

        if(continuar) apagar[y] = 1;
    }

    for(int i=ALTURA-2; i>=0; i--){
        if(apagar[i]){
            pontos += 100;
            deslocar_linha(i);
            apagou=1;
        }
    }

    return apagou;
}

void comandos(){
    char c;
    if(read(STDIN_FILENO, &c, 1) > 0){
        switch(c){
            case 'a': movimentar(0); break;
            case 'd': movimentar(1); break;
            case 's': movimentar(2); break;
            case 'k': rotacionar(1); break;
            case 'l': rotacionar(0); break;
        }
    }
}

int derrota(){
    /*
        fim = existem blocos congelados acima da linha limite
    */
    for(int y=0; y<=LINHA_LIMITE; y++){
        for(int x=1; x<LARGURA-1; x++){
            if(mapa[y][x].tipo == 2) return 1;
        }
    }
    return 0;
}

void imprimir(){
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            if(proximo.uso[i][j]){
                printf("%s  "RESET, cores[proximo.cor]);
            }
            else printf(" ");
        }
        printf("\n");
    }
    
    printf("\n\n");

    for(int y=0; y<ALTURA; y++){
        for(int x=0; x<LARGURA; x++){
            int imp=0;

            for(int i=0; i<4 && !imp; i++){
                for(int j=0; j<4 && !imp; j++){
                    if(bloco.uso[i][j]){
                        if(y == bloco.p.y + i && x == bloco.p.x + j){
                            printf("%s  "RESET, cores[bloco.cor]);
                            imp=1;
                        }
                    }
                }
            }

            if(!imp) printf("%s%c "RESET, cores[mapa[y][x].cor], Mapa[y][x]);
        }
        printf("\n");
    }

    printf("Pontos: %d     \n", pontos);
}

void info_para_debug(){
    for(int i=0; i<LARGURA; i++){
        printf("%d ", mapa[ALTURA-1][i].tipo);
    }
    printf("\n");

    for(int i=0; i<LARGURA; i++){
        printf("%d ", mapa[ALTURA-2][i].tipo);
    }

    printf("\n\nCoordenada: (%d, %d)  \n", bloco.p.x, bloco.p.y);
    printf("Cor: %d  \n", bloco.cor);
}