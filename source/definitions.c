#include "settings.h"

#ifndef DEFINITIONS
#define DEFINITIONS

Blocos bloco;//só um "existe" por vez
Blocos proximo = {0};
Celulas mapa[ALTURA][LARGURA];//def

Coordenada direcao[3] = {//def
    {-1, 0},//esquerda
    {1, 0},//direita
    {0, 1},//baixo
};

int pontos=0;//def

char cores[][10] = {
    BRANCO_VIB,
    FUNDO_VERM,
    FUNDO_VERDE,
    FUNDO_AMARELO,
    FUNDO_AZUL,
    FUNDO_ROXO,
    FUNDO_CIANO,
};

#endif