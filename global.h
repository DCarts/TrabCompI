/*
 * defs.h
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */

#ifndef GLOBALDEFS_H
#define GLOBALDEFS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "defs.h"


/* Constantes */
extern const int true;
extern const int false;

extern const int BLOCK_DIST;
extern const int BLOCKS_W;
extern const int BLOCKS_H;

extern const int STD_SCREEN_WIDTH;
extern const int STD_SCREEN_HEIGHT;
extern const int OFFSET;

extern const int PLAT_SPD;

extern const int MAX_NUM_BOLAS;

extern const int SOUND_WALL;
extern const int SOUND_TETO;
extern const int SOUND_FLOOR;

/* Variaveis */

extern int gScoreBoardHeight;
extern int gScoreBoardWidth;
extern int gScreenWidth;
extern int gScreenHeight;

extern SDL_Window* gWindow;
extern SDL_Surface* gScreenSurface;

extern SDL_Surface* gBlocoImgs[10];
extern SDL_Surface* gBlocoBreak;
extern SDL_Surface* gBallImgs[5];
extern SDL_Surface* gPadImgs[5];
extern SDL_Surface* gTexto;
extern SDL_Surface* gPontos;

extern SDL_Color corDaFonte;
extern SDL_Color backgroundColor;

extern Mix_Chunk* gSons[10];

extern TTF_Font* gFonte;

extern int gNumBolas;
extern int gNumBlocos;

extern PLATAFORMA* gPad;
extern BOLA* gBolas;
extern BLOCO* gBlocos;
extern PLAYER gPlayer;

extern int gLeft, gRight;
extern int gXMouse, gYMouse;

#endif
