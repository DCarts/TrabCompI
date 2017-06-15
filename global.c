/*
 * global.c
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "defs.h"
#include "global.h"

const int true = 1;
const int false = 0;

const int BLOCK_DIST = 6;
const int BLOCKS_W = 15;
const int BLOCKS_H = 10;

const int STD_SCREEN_WIDTH = 640;
const int STD_SCREEN_HEIGHT = 480;
const int OFFSET = 32;

const int PLAT_SPD = 100;

const int MAX_NUM_BOLAS = 50;

const int SOUND_WALL = 0;
const int SOUND_TETO = 1;
const int SOUND_FLOOR = 2;

int gScoreBoardHeight = 30;
int gScoreBoardWidth = 110;
int gScreenWidth = 750; /*640*/
int gScreenHeight = 480; /*480*/

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gBlocoImgs[10];
<<<<<<< HEAD
SDL_Surface* gBlocoCracks;
=======
SDL_Surface* gBlocoBreak;
>>>>>>> 4a8ae2a9403c86a40fc51a73062e08b4d75e84bc
SDL_Surface* gBallImgs[5];
SDL_Surface* gPadImgs[5];
SDL_Surface* gTexto = NULL;
SDL_Surface* gPontos = NULL;

SDL_Color corDaFonte = {255,255,255};
SDL_Color backgroundColor = {0,0,0};

Mix_Chunk* gSons[10];

TTF_Font* gFonte = NULL;

int gNumBolas = 6;
int gNumBlocos = 0;

PLATAFORMA* gPad = NULL;
BOLA* gBolas = NULL;
BLOCO* gBlocos = NULL;
PLAYER gPlayer;

int gLeft = 0, gRight = 0;
int gXMouse = 0, gYMouse = 0;
