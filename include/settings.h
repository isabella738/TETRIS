#ifndef SETTINGS

#define SETTINGS

#define LARGURA 21
#define ALTURA 24
#define MAX_BLOCOS 7
#define MAX_CORES 6
#define LINHA_LIMITE 3

#define APAGAR "\033[0J"//apaga a tela da posição do cursor para baixo
#define VOLTAR "\033[H"//volta para o inicio do terminal
#define TEMPO_DESCIDA 10

#define RESET "\033[0m"
#define FUNDO_VERM "\033[41m"
#define FUNDO_VERDE "\033[42m"
#define FUNDO_AMARELO "\033[43m"
#define FUNDO_AZUL "\033[44m"
#define FUNDO_ROXO "\033[45m"
#define FUNDO_CIANO "\033[46m"
#define BRANCO_VIB "\033[97m"

typedef struct{
    int x, y;
}Coordenada;

typedef struct{
    int tipo; //0 = vazoi; 1 = parede; 2 = bloco
    int cor;
}Celulas;

typedef struct{
    Coordenada p;
    int uso[4][4];
    int cor;
}Blocos;

extern char cores[][10];
extern Blocos bloco;
extern Blocos proximo;
extern Celulas mapa[ALTURA][LARGURA];
extern Coordenada direcao[3];
extern int pontos;

#endif