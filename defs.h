/*
 * defs.h
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */

#ifndef GAMEDEFS_H
#define GAMEDEFS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/* Macros */
#define GAME_PI 3.14159265358979323846

/* Structs */
typedef struct {
	double x;
	double y;
} VETOR2D;

typedef struct {
	VETOR2D pos;
	VETOR2D dir;
	VETOR2D prevPos;
	int tipo;
	int ativo;
	int dim;
	double spd;
	double lastDelta;
	SDL_Surface* img;
	int colada;
} BOLA;

typedef struct {
	VETOR2D pos;
	VETOR2D dir;
	int melhorias[5];
	int ativo;
	int w;
	int h;
	double spd;
	SDL_Surface* img;
} PLATAFORMA;

typedef struct {
	VETOR2D pos;
	int tipo;
	int w, h;
	int maxVida;
	int vida;
	SDL_Surface* img;
} BLOCO;

typedef struct {
	int vidas;
	int pontos;
} PLAYER;

typedef struct {
	VETOR2D pos;
	VETOR2D dir;
	VETOR2D prevPos;
	int tipo;
	double spd;
	double lastDelta;
	int dim;
	int ativo;
	SDL_Surface* img;
} PWP;


typedef struct {
	char* name;
	int pts;
	long int sysTime;
} SCOREENTRY;

#endif
