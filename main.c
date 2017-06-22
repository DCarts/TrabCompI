/*
 * main.c
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 *
 * 		AS VELOCIDADES SÃO POR SEGUNDO
 * 		Px / Segundo
 */

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "defs.h"
#include "global.h"
#include "game.h"
#include "media.h"
#include "render.h"
#include "afterall.h"

int main(int argc, char **argv) {
	int quit, count;
	long countTime, currentTime, lastTime;
	double delta;
	SDL_Event evt;

	//gGameHeight += gScoreBoardHeight;/*aumentei para caber o scoreboard na tela*/
	gScoreOffset += gGameWidth;

	gScreenWidth = gScoreOffset+gScoreWidth;
	gScreenHeight = gGameHeight;

	if (!init()) {
		return 1;
	}

	atexit(exitGame);

	if (!loadMedia()) {
		return 1;
	}

	createNPCs();
	loadBlocosFromFile("teste");

	quit = false;
	currentTime = countTime = SDL_GetTicks();
	countTime++;

	gLeft = false;
	gRight = false;

	/* Timer do jogo */
	while (!quit) {
		while (SDL_PollEvent(&evt) != 0) {
			quit = handleEvent(&evt);
		}
		lastTime = currentTime;
		currentTime = SDL_GetTicks();
		delta = (currentTime-lastTime)/1000.0;
		//printf("%.1f\n",currentTime/1000.0);
		if (countTime < currentTime) {
			printf("FPS=%d\n", count);
			count = 0;
			countTime = currentTime + 1000;
		}
		if (gameLoop(delta)) break;
		if (render()) break;
		count++;
	}

	exitGame();
	createPlayer();

	return !quit;
}
