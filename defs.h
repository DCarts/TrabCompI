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

/* Structs */
typedef struct {
	double x;
	double y;
} VETOR2D;

typedef struct {
	VETOR2D pos;
	VETOR2D dir;
	int tipo;
	int ativo;
	int dim;
	double spd;
	SDL_Surface* img;
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
	char nome[22];
} PLAYER;

typedef struct {
	VETOR2D pos;
	VETOR2D dir;
	int tipo;
	int ativo;
	int dim;
	double spd;
	SDL_Surface* img;
} PWP;

#endif
