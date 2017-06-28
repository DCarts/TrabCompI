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
PWP createPwp(VETOR2D pos, VETOR2D dir, int tipo, int dim, double spd, int ativo, SDL_Surface* img);

/* Cria o player atual */
void createPlayer();

/* Move uma bolinha */
void moveBall(BOLA* b, double delta);

/* Move uma melhoria (powerup) */
void movePwp(PWP* pwp, double delta);

/* Move a plataforma */
void movePlataforma(PLATAFORMA *p, double delta);


/* Loop do jogo */
int gameLoop();

/* Destruicao de adjacentes indesejados */
void destroiVizinhos(BLOCO* b);

/* Processa eventos */
int handleEvent(SDL_Event* e);
int handleInput(SDL_Event* evt);

/* Vai para o proximo nivel */
/* Retorna true se foi para o proximo nivel com sucesso, false se ja tava no ultimo */
int goToNextLevel();

/* Encerra os sistemas e sai do jogo */
void exitGame();

/* Funcoes de colisao */
int collPwpPlat(PWP* p, PLATAFORMA* b, double delta);
int collBallPlat(BOLA* a, double delta);
int collBallBlock(BOLA* a, BLOCO* b, double delta);
int collBallPoint(BOLA* a, double dx, double dy, double delta);

/* modifica a dir. de uma bola qnd colide com a plataforma */
void platModify(BOLA* b);

/* Duplica as bolas existentes */
void dupaBalls();

/* Clona uma bola existente */
BOLA cloneBola(BOLA* orig);

#endif
