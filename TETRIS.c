#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
/*
100 pontos por linha eliminada
Obs: evitar rotacionar blocos muito próximo de outros
*/

#define APAGAR "\033[0J"//apaga a tela da posição do cursor para baixo
#define VOLTAR "\033[H"//volta para o inicio do terminal
#define TEMPO_DESCIDA 100

#define VERMELHO "\033[31m"
#define VERDE  "\033[32m"
#define AMARELO "\033[33m"
#define AZUL "\033[34m"
#define MAGENTA "\033[35m"
#define CIANO "\033[36m"
#define BRANCO "\033[37m"
#define CINZA "\033[90m"
#define RESET  "\033[0m"

struct termios velho_terminal, novo_terminal;
void mudar_terminal(){
    tcgetattr(STDIN_FILENO, &velho_terminal);
    novo_terminal = velho_terminal;

    novo_terminal.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &novo_terminal);
}
void restaurar_terminal(){
    tcsetattr(STDIN_FILENO, TCSANOW, &velho_terminal);
}

//Estruturas
typedef struct{
    int x, y;
}coordenada;

typedef struct{
    char simb;
    char cor[9];
}celula;

typedef struct{
    coordenada coord;
    coordenada uso[4]; //hitbox, em relação à propria matriz, não o mapa
    char figura[4][4];
    char cor[9];
}Bloco;

char cor[][9]={//0branco, 1cinza, 2amarelo, 3verde, 4vermelho, 5azul
    BRANCO,
    CINZA,
    AMARELO,
    VERDE,
    VERMELHO,
    AZUL,
    MAGENTA,
    CIANO
};

char blocos[7][4][5]={
    {
        "#   ",
        "#   ",
        "#   ",
        "#   ",
    },
    {
        "##  ",
        "##  ",
        "    ",
        "    "
    },
    {
        "#   ",
        "#   ",
        "##  ",
        "    ",

    },
    {
        " #  ",
        " #  ",
        "##  ",
        "    ",

    },
    {
        " #  ",
        "### ",
        "    ",
        "    ",
    },
    {
        "#   ",
        "##  ",
        " #  ",
        "    ",
    },
    {

        " #  ",
        "##  ",
        "#   ",
        "    ",
    },
};

char mapa[21][22]={//mapa 19x19 sem bordas, 21x21 com bordas
    " ___________________ ",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "|                   |",
    "+===================+",
};

char inicio[7][49]={//mapa 19x19 sem bordas, 21x21 com bordas
    " +============================================+ ",
    "||   _____   ___   _____   ____   ___   __    ||",
    "||  |__ __| | __| |__ __| |    | |_ _| |  _|  ||",
    "||    | |   | _|    | |   |   .   | |   \\ \\   ||",
    "||    |_|   |___|   |_|   |_|\\_\\ |___| |___|  ||",
    "||                                            ||",
    " +============================================+"
};


celula cel[21][21];

void rotacionar(Bloco *b){
    
    //Rotacionar figura
    char novo[4][4]; int u=0;
    for(int y=0; y<4; y++){
        for(int x=0; x<4; x++){
            novo[y][x] = b->figura[3-x][y];

            if(novo[y][x]=='#'){
                b->uso[u].x=x; b->uso[u].y=y; u++;
            }
        }
    }

    //Concertar posição, caso estoure os limites do mapa
    int puxar_direita=0, puxar_esquerda=0;
    for(int i=0; i<4; i++){
        int x_global = b->uso[i].x + b->coord.x;

        if(x_global <= 1){

            int d = 1 - x_global;
            if(d > puxar_direita) puxar_direita=d;
        }
        else if(x_global >= 19){
            
            int d = x_global - 19;
            if(d > puxar_esquerda) puxar_esquerda=d;
        }
    }
    if(puxar_direita){
        b->coord.x += puxar_direita;
    }
    else if(puxar_esquerda){
        b->coord.x -= puxar_esquerda;
    }
    
    for(int i=0; i<4; i++)memcpy(b->figura[i], novo[i], sizeof(novo[i]));
}

void congelar(celula cel[][21], Bloco *b){
    int x_global = b->coord.x;
    int y_global = b->coord.y;

    for(int i=0; i<4; i++){
        int x_local = b->uso[i].x;
        int y_local = b->uso[i].y;

        cel[y_local+y_global][x_global+x_local].simb = '#';
        strcpy(cel[y_local+y_global][x_global+x_local].cor, b->cor);
    }
}

int main(){
    system("clear");

    for(int i=0; i<7; i++) printf("%s\n", inicio[i]);
    printf("\nPressione enter para comecar.\n"); getchar();
    system("clear");

    srand(time(NULL));
    mudar_terminal();
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    Bloco bloco;

    //Inicialização
    for(int y=0; y<21; y++) for(int x=0; x<21; x++){
        cel[y][x].simb = mapa[y][x];
        strcpy(cel[y][x].cor, BRANCO);
    }
    int proximo_bloco = rand()%7;
    int proxima_cor = rand()%6+2;

    int novo_bloco=1, cooldown = TEMPO_DESCIDA, frames=0, perdeu=0, pontos=0;
    int apagar_lin[21], linha=0;//apagar_lin: posição y das linhas a serem apagadas; linha: a quantidade
    do{
        printf(VOLTAR); usleep(10000);

        int esquerda=0, direita=0;
        frames++; cooldown--;

        //spawn de novo bloco
        if(novo_bloco){

            //Salva a coordenada das celulas significativas
            int u=0;
            for(int i=0; i<4; i++) for(int j=0; j<4; j++) {
                bloco.figura[i][j] = blocos[proximo_bloco][i][j];
                if(bloco.figura[i][j]=='#') {
                    bloco.uso[u].x=j; bloco.uso[u].y=i; u++;

                }
            }
            strcpy(bloco.cor, cor[proxima_cor]);
            bloco.coord.x=10;
            bloco.coord.y=1;

            proximo_bloco = rand()%7; //escolhe bloco seguinte
            proxima_cor = rand()%6+2; //escolher cor seguinte
            novo_bloco=0;
        }
        
        int *xB = &bloco.coord.x; //bloco.coord.x
        int *yB = &bloco.coord.y; //bloco.coord.y

        //colisão e descida
        int colisao=0;
        if(!cooldown){
            //colisão
            for(int i=0; i<4; i++){
                int x_local = bloco.uso[i].x;
                int y_local = bloco.uso[i].y;

                if(cel[y_local + *yB+1][x_local + *xB].simb!=' '){
                    congelar(cel, &bloco);
                    novo_bloco=1;
                    colisao=1;
                    cooldown = TEMPO_DESCIDA;
                    break;
                }
            }

            //Descem os blocos
            if(linha){
                for(int i=0; i<linha; i++){
                    for(int y=apagar_lin[i]; y>0; y--){
                        for(int x=1; x<=19; x++) cel[y][x] = cel[y-1][x];
                    }
                }
                linha=0;
            }
            
            if(!colisao){
                (*yB)++; cooldown = TEMPO_DESCIDA;
            }
        }
        
        //Verifica se já chegou no topo
        if(colisao) for(int x=1; x<=19; x++){
            if(cel[1][x].simb=='#'){
                perdeu=1; break;
            }
        }
        if(perdeu)break;

        //Identificar linhas completas
        for(int y=1; y<=19; y++){
            int cheio=1;
            for(int x=1; x<=19; x++){
                if(cel[y][x].simb == ' '){
                    cheio=0; break;
                }
            }
            if(cheio){apagar_lin[linha]=y; pontos+=100; linha++;}
        }

        //Apagar linhas
        if(linha){
            for(int i=0; i<linha; i++) for(int x=1; x<=19; x++){
                cel[apagar_lin[i]][x].simb = ' ';
            }
        }

        //comandos do jogador
        char comando, parar=0;
        if(read(STDIN_FILENO, &comando, 1) > 0){
            switch(comando){
                case 'a': 
                    for(int i=0; i<4; i++){
                        if(cel[bloco.uso[i].y + *yB][bloco.uso[i].x + *xB -1].simb!=' '){ 
                            parar=1; break;
                        }
                    }
                    if(!parar)(*xB)--; 
                    break;
                case 'd': 
                    for(int i=0; i<4; i++){
                        if(cel[bloco.uso[i].y + (*yB)][bloco.uso[i].x + *xB +1].simb!=' '){ 
                            parar=1; break;
                        }
                    }
                    if(!parar)(*xB)++; 
                    break;

                case 's':
                    for(int i=0; i<4; i++){
                        if(cel[bloco.uso[i].y + (*yB) +1][bloco.uso[i].x + *xB].simb!=' '){ 
                            parar=1; break;
                        }
                    }
                    if(!parar)(*yB)++; 
                    break;

                case 'k': esquerda=1; break;
                case 'l': direita=1; break;
            }
        }

        //rotação
        if(esquerda) rotacionar(&bloco);
        if(direita) for(int i=0; i<3; i++) rotacionar(&bloco);

        //Impressão
        printf("Proximo bloco:\n");
        for(int i=0; i<4; i++) printf("%s%s\n"RESET, cor[proxima_cor], blocos[proximo_bloco][i]);
        printf("\nPontuacao: %d      \n", pontos);
        for(int y=0; y<21; y++){
            for(int x=0; x<21; x++){
                int imp=0;
                
                //Bloco em movimento, só imprime os que não são espaços
                if(x>=bloco.coord.x && x<bloco.coord.x+4 && y>=bloco.coord.y && y<bloco.coord.y+4){
                    if(bloco.figura[y-bloco.coord.y][x-bloco.coord.x]!=' '){
                        printf("%s#"RESET, bloco.cor); imp=1;
                    }
                }
                if(!imp) printf("%s%c"RESET, cel[y][x].cor, cel[y][x].simb);//resto

            }
            printf("\n");
        }
        printf("\nLinha: %d  \n", linha);
    }while(1);
    system("clear");
    for(int y=0; y<21; y++){
        for(int x=0; x<21; x++){
            printf("%s%c"RESET, CINZA, cel[y][x].simb);
        }
        printf("\n");
    }
    printf("\nFim de Jogo.\n");
    printf("Pontuacao: %d\n", pontos);

    return 0;
}