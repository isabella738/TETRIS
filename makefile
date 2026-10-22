all:
	gcc -I./include source/allmain.c source/definitions.c source/functions.c source/sprites.c source/terminal.c -o TETRIS