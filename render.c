/*
 * global.c
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "defs.h"
#include "global.h"
#include "render.h"

static SDL_Rect srcRect, dstRect;

int render() {
	int i, err = false;

	/* Desenha area de jogo */
	SDL_FillRect( gScreenSurface, NULL,
	SDL_MapRGB( gScreenSurface->format,
				0, 0, 0 ) );

	srcRect.x = 24; srcRect.y = 24;
	srcRect.w = 592; srcRect.h = 432;

	SDL_FillRect( gScreenSurface, &srcRect,
	SDL_MapRGB( gScreenSurface->format,
				0xBB, 0xBB, 0xBB ) );

	srcRect.x = 32; srcRect.y = 32;
	srcRect.w = 576; srcRect.h = 416;

	SDL_FillRect( gScreenSurface, &srcRect,
	SDL_MapRGB( gScreenSurface->format,
				0, 0, 0 ) );

	/* Renderiza os blocos */
	for(i = 0; i < gNumBlocos; i++) {
		if (gBlocos[i].vida){

			srcRect.y = 0;
			srcRect.w = gBlocos[i].w;
			srcRect.h = gBlocos[i].h;

			srcRect.x = 0; srcRect.y = 0;

			dstRect.x = gBlocos[i].pos.x;
			dstRect.y = gBlocos[i].pos.y;

			srcRect.x = 0; srcRect.y = 0;

			if( SDL_BlitSurface( gBlocos[i].img, &srcRect,
								gScreenSurface, &dstRect ) < 0 ) {
				fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
				err = true;
				gGameStatus = -201;
			}

			if (gBlocos[i].vida < gBlocos[i].maxVida){
				srcRect.x = 32 * (4 - gBlocos[i].vida); srcRect.y = 0;

				if( SDL_BlitSurface( gBlocoBreak, &srcRect,
									gScreenSurface, &dstRect ) < 0 ) {
					fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
					gGameStatus = -202;
					err = true;
				}
			}
		}
	}

	/* Renderiza os leds */
	srcRect.x = 0; srcRect.y = 0;

	for(i = 0; i < MAXVIDAS; i++) {
		srcRect.w = 24;
		srcRect.h = 8;

		dstRect.x = gGameWidth + i*32;
		dstRect.y = gScreenHeight - 2*OFFSET;

		if (i+1 <= gPlayer.vidas){
			if( SDL_BlitSurface( gLed[0], &srcRect,
								gScreenSurface, &dstRect) < 0 ) {
				fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
				gGameStatus = -203;
				err = true;
			}
		}
		else {
			if( SDL_BlitSurface( gLed[1], &srcRect,
								gScreenSurface, &dstRect) < 0 ) {
				fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
				gGameStatus = -204;
				err = true;
			}
		}
	}


	/* Renderiza as bolas */
	for(i = 0; i < gNumBolas; i++) {
		srcRect.w = gBolas[i].dim;
		srcRect.h = gBolas[i].dim;

		dstRect.x = gBolas[i].pos.x;
		dstRect.y = gBolas[i].pos.y;

		if (gBolas[i].ativo){
			if( SDL_BlitSurface( gBolas[i].img, &srcRect,
								gScreenSurface, &dstRect) < 0 ) {
				fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
				gGameStatus = -205;
				err = true;
			}
		}
	}

	/* Renderiza a plataforma */
	srcRect.w = 24;
	srcRect.h = gPad[0].h;

	dstRect.x = gPad[0].pos.x;
	dstRect.y = gPad[0].pos.y;

	srcRect.x = 0;

	/* Ponta da esquerda */
	if( SDL_BlitSurface( gPad[0].img, &srcRect,
								gScreenSurface, &dstRect ) < 0 ) {
				fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
				gGameStatus = -206;
				err = true;
	}

	/* Meio (de acordo com a quantidade de pedacos) */
	for(i = 1; i <= gPad[0].pedacos; i++){

		srcRect.x = 24;
		dstRect.x = gPad[0].pos.x + 24*i;

		if( SDL_BlitSurface( gPad[0].img, &srcRect,
								gScreenSurface, &dstRect ) < 0 ) {
				fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
				gGameStatus = -206;
				err = true;
			}
	}

	dstRect.x = gPad[0].pos.x + 24*i;
	srcRect.x = 72;

	/* Ponta da direita */
	if( SDL_BlitSurface( gPad[0].img, &srcRect,
								gScreenSurface, &dstRect ) < 0 ) {
				fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
				gGameStatus = -206;
				err = true;
	}

	srcRect.x = 0; srcRect.y = 0;

	/* Renderiza o powerup */
	if (gPowerUp.ativo){
		srcRect.w = gPowerUp.dim;
		srcRect.h = gPowerUp.dim;

		dstRect.x = gPowerUp.pos.x;
		dstRect.y = gPowerUp.pos.y;

		if( SDL_BlitSurface( gPowerUp.img, &srcRect,
								gScreenSurface, &dstRect ) < 0 ) {
				fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
				gGameStatus = -206;
				err = true;
			}
	}

	if (!err)
		err = renderScoreboard();

    /*	Update the surface	*/
    SDL_UpdateWindowSurface( gWindow );

	return err;
}

int renderScoreboard() {
	int err = false;
	static int lastScore = -1;
	int lastLevel = -1;
	static char lvlStr[4];
	/*	static char scoreText[8];	*/

	if (gPlayer.pontos != lastScore) {
		lastScore = gPlayer.pontos;
		sprintf(gScoreText, "%d pts", lastScore);

		SDL_FreeSurface(gScoreSurface);

		gScoreSurface = NULL;
		if(!(gScoreSurface = TTF_RenderText_Shaded(gScoreFonte, gScoreText, gScoreFontColor, gBgColor))) {
			fprintf(stderr,"Impossivel renderizar texto na tela! %s\n",TTF_GetError());
			gGameStatus = -207;
			err = true;
		}
	}

	if(gLvlNumber != lastLevel)
	{
		lastLevel = gLvlNumber;
		sprintf(lvlStr, "lvl %d", lastLevel);

		SDL_FreeSurface(gLvlNumberSurface);
		gLvlNumberSurface = NULL;

		if(!(gLvlNumberSurface = TTF_RenderText_Shaded(gScoreFonte, lvlStr, gScoreFontColor, gBgColor))) {
			fprintf(stderr,"Impossivel renderizar gLvlNumberSurface na superfície! %s\n",TTF_GetError());
			gGameStatus = -208;
			err = true;
		}
	}


	dstRect.x = gScoreOffset;
	dstRect.y = 16;
	dstRect.w = 128;
	dstRect.h = 48;

	if (SDL_BlitSurface(gScoreSurface, NULL, gScreenSurface, &dstRect) < 0) {
		fprintf(stderr,"Impossivel blitar textoB na tela! %s\n",SDL_GetError());
		gGameStatus = -208;
		err = true;
	}

	/*dstRect.x = gScoreOffset;*/
	dstRect.y += 72;
	/*dstRect.w = 128;
	dstRect.h = 48;*/

	if (SDL_BlitSurface(gLvlNumberSurface, NULL, gScreenSurface, &dstRect) < 0) {
		fprintf(stderr,"Impossivel blitar LVLNUMBER na tela! %s\n",SDL_GetError());
		gGameStatus = -208;
		err = true;
	}
	return err;
}
