 /*
 * mainmenu.h
 * Aqui estão definidas as funçoes do menu principal do jogo
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */


#ifndef MAINMENU_H
#define MAINMENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "defs.h"
#include "global.h"

/* Lidar com o menu inicial do jogo */
int menu(void);

/*	Liberar as superfícies que foram usadas na função Menu */
void freeMenu(void);

/*Apresentar o ranking dos melhores jogadores */
int showRanking(void);

/*retorna 0 se for de boas,retorna conforme o codigo de erro */
int renderAndBlit(int);

/*fecha as superficies do ranking*/
void freeRanking(int);

/*Blita o nome dos participantes na tela*/
void blitParts(void);

/*	Libera as superfícies parts */
void freeParts(void);
#endif
