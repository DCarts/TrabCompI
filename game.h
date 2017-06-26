/*
 * defs.h
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */

#ifndef BREAKOUTDEFS_H
#define BREAKOUTDEFS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "defs.h"
#include "global.h"

/* Inicia o jogo */
int init();

/* Cria os NPCs */
int createNPCs();

/* Cria uma bolinha */
BOLA createBola(VETOR2D pos, VETOR2D dir, int tipo, int dim, double spd, SDL_Surface* img);

/* Cria um bloco */
BLOCO createBloco(VETOR2D pos, int tipo, int w, int h, int v, SDL_Surface* img);

/* Cria a plataforma */
PLATAFORMA createPlataforma(VETOR2D pos, VETOR2D dir, SDL_Surface* img);

/* "Cria" um powerup */
PWP createPwp(VETOR2D pos, VETOR2D dir, int tipo, double spd, int ativo, SDL_Surface* img);

/* Cria o player atual */
void createPlayer();

/* Move uma bolinha */
void moveBall(BOLA* b, double delta);

/* Move a plataforma */
void movePlataforma(PLATAFORMA *p, double delta);


/* Loop do jogo */
int gameLoop();


/* Processa eventos */
int handleEvent(SDL_Event* e);
int handleInput(SDL_Event* evt);

/* Vai para o proximo nivel */
/* Retorna true se foi para o proximo nivel com sucesso, false se ja tava no ultimo */
int goToNextLevel();

/* Encerra os sistemas e sai do jogo */
void exitGame();

/* Funcoes de colisao */
int collBallPlat(BOLA* a, double delta);
int collBallBlock(BOLA* a, BLOCO* b, double delta);
int collBallPoint(BOLA* a, double dx, double dy, double delta);

#endif
