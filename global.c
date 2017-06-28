/*
 * global.c
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "defs.h"
#include "global.h"

const int true = 1;
const int false = 0;

const double tol = 1; /*tolerancia nas comparacoes*/

const int BLOCK_DIST = 6;
const int BLOCKS_W = 15;
const int BLOCKS_H = 10;

const int STD_SCREEN_WIDTH = 640;
const int STD_SCREEN_HEIGHT = 480;
const int OFFSET = 32;

const int PLAT_SPD = 100;

const int MAX_NUM_BOLAS = 8;

const int SOUND_WALL = 0;
const int SOUND_TETO = 1;
const int SOUND_FLOOR = 2;
const int SOUND_PLAT = 3;
const int SOUND_BLOCK_BROKE = 4;
const int SOUND_BLOCK_HIT = 5;
const int SOUND_LIFE_LOST = 6;
const int SOUND_LIFE_GAIN = 7;
const int SOUND_GAMEOVER = 8;
const int SOUND_GAMESTART = 9;

const int MAXVIDAS = 4;

int flip;

/*
 * < 0 = erro
 * 0 = começando
 * 1 = rodando
 * > 99 = saindo
 * 100 = jogo saindo pq deu gameover (nesse caso a gente chama a main dnv ehuaheauhe
 * 200 = jogo saindo no meio (esc ou X)
 * 300 = saindo da tela de salvar (sem salvar)
 * _01 = salvar score
 * _02 = salvando score
 * _03 = score salvo
 *
 */
int gGameStatus = 0;
int gLvlNumber;

int gScoreOffset = 0;
int gScoreWidth = 128;
int gGameWidth = 640;
int gGameHeight = 480;

int gScreenWidth = 0;
int gScreenHeight = 0;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gBlocoImgs[10];
SDL_Surface* gPWPImgs[10];
SDL_Surface* gBlocoBreak;
SDL_Surface* gBallImgs[5];
SDL_Surface* gPadImgs[5];
SDL_Surface* gLed[2];

Mix_Chunk* gSons[10];

SDL_Surface* gScoreSurface = NULL;
TTF_Font* gScoreFonte = NULL;
TTF_Font* gHiScoreFonte = NULL;

SCOREENTRY gPlayers[6];

int gNumBolas;
int gNumBlocos;
int gNumBlocosAlive;
int gAllPts = 0;
int gTimesPlayed = 0;/* talvez isso seja util */
int gGameOver = 0;/*indica se o jogador perdeu todas as vidas */

PLATAFORMA* gPad = NULL;
BOLA* gBolas = NULL;
BLOCO* gBlocos = NULL;
PLAYER gPlayer;
PWP gPowerUp;

int gLeft = 0, gRight = 0;
int gXMouse = 0, gYMouse = 0;

FILE *gRank;/* arquivo que armazena a pontuação do jogador */
char gScoreText[12];

SDL_Color gScoreFontColor = {255,255,255};
SDL_Color gBgColor = {0,0,0};
