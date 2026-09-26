#include "settings.h"
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>

struct termios velho_terminal, novo_terminal;//def
void mudar_terminal(){//def
    tcgetattr(STDIN_FILENO, &velho_terminal);
    novo_terminal = velho_terminal;

    novo_terminal.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &novo_terminal);
}
void restaurar_terminal(){
    tcsetattr(STDIN_FILENO, TCSANOW, &velho_terminal);
}

void pausa(){
    printf("\nPressione qualquer tecla para continuar.\n");
    getchar();
}
