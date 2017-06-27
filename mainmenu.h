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
void showRanking(void);

#endif