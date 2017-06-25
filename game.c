/*
 * global.c
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */

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
#include "render.h"
#include "util.h"
#include "media.h"
#include "afterall.h"

int init() {
	int imgFlags = IMG_INIT_PNG;
	srand(time(NULL));

	/* Inicializa o SDL */
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Erro: SDL falhou ao iniciar: %s\n",
			SDL_GetError());
		return false;
	}

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
		fprintf(stderr, "Erro: SDL_mixer nao conseguiu inicializar: %s\n",
			Mix_GetError());
		return false;
	}

	if(TTF_Init() < 0){
		fprintf(stderr,"Erro: SDL_ttf nao conseguiu inicializar: %s\n",
		TTF_GetError());
		return false;
	}

	Mix_AllocateChannels(8); /* Aloca 8 canais de audio pro jogo, sem contar musicas */

	gWindow = SDL_CreateWindow("Breakout Work-in-Progress",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		gScreenWidth, gScreenHeight,
		SDL_WINDOW_SHOWN);

	if (!gWindow) {
		fprintf(stderr, "Erro: Janela nao pode ser criada: %s\n",
		SDL_GetError());
		return false;
	}

	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fprintf(stderr, "Erro: SDL_image nao conseguiu inicializar: %s\n",
			SDL_GetError());
		return false;
	}
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	return true;
}

void moveBall(BOLA* p, double delta) {

	p->prevPos.x = p->pos.x;
	p->prevPos.y = p->pos.y;

	p->pos.x += p->dir.x*p->spd*delta;
	p->pos.y += p->dir.y*p->spd*delta;
	
	//printf("px=%.3lf, py=%.3lf\n", p->prevPos.x, p->prevPos.y);
	//printf("ox=%.3lf, oy=%.3lf\n\n", p->pos.x, p->pos.y);


	if (p->pos.x + p->dim > gGameWidth-OFFSET || p->pos.x < OFFSET) {
		p->dir.x = -p->dir.x;
		p->pos.x = p->prevPos.x;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], 0); */
	}
	if (p->pos.y + p->dim > gGameHeight-OFFSET) {
		/*jogador perde pontos*/

		gPlayer.pontos -= 1000;
		gPlayer.vidas--;
		if(gPlayer.pontos < 0){
			gPlayer.pontos = 0;
		}

		p->ativo = false;
		p->dir.y = -p->dir.y;
		p->pos.y = p->prevPos.y;
		/* Mix_PlayChannel(-1, gSons[SOUND_FLOOR], 0); */
	}
	else if (p->pos.y < 32) {
		p->dir.y = -p->dir.y;
		p->pos.y  = p->prevPos.y;
		/* Mix_PlayChannel(-1, gSons[SOUND_TETO], 0); */
	}
}

void movePlataforma (PLATAFORMA* p, double delta) {

	if (gRight) {
		p->spd += delta * 9;
		if (p->spd > p->dir.x)
			p->spd = p->dir.x;
	}

	else if (gLeft) {
		p->spd -= delta * 9;
		if (p->spd < -p->dir.x)
			p->spd = -p->dir.x;
	}
	else if (p->spd != 0) {
		p->spd += (p->spd < 0) ?
			delta * 9 : delta * -9;
	}


	p->pos.x += p->spd*PLAT_SPD*delta;


	if (p->pos.x + p->w > gGameWidth-OFFSET) {
		p->spd = 0;
		p->pos.x = gGameWidth-OFFSET-p->w;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], 0); */ /* nao seria uma boa ideia colocar o som de colisao da bolinha pra plataforma */
	}

	if (p->pos.x < OFFSET) {
		p->spd = 0;
		p->pos.x = OFFSET;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], 0); */ /*idem*/
	}
}

int gameLoop(double delta) {
	int i, j;

	movePlataforma(gPad, delta);

	for (i = 0; i < gNumBolas; i++) {
		if (gBolas[i].ativo){

			/* if bola is colada continue */

            moveBall(gBolas+i, delta);
			collBallPlat(gBolas+i, delta);

			for (j = 0; j < gNumBlocos; j++) {
				if (gBlocos[j].vida) {
					if (collBallBlock(gBolas+i, gBlocos+j, delta)) {
						gPlayer.pontos += 100;
						if(gPlayer.pontos % 10000 == 0)
						{
							gPlayer.vidas++;
						}
						//if (gBlocos[j].vida == 1 && !(rand() % 5))
						//	createPwp();
					}
				}
			}
		}
		if(gPlayer.vidas == -1)
		{
			SDL_UpdateWindowSurface(gWindow);
			exitGame();
		}
	}

	return false;
}

BOLA createBola(VETOR2D pos, VETOR2D step, int tipo, int dim, double spd, SDL_Surface* img) {
	BOLA bola;
	bola.pos = pos;
	bola.prevPos = copyVector(pos);
	normalize(&step);
	bola.dir = step;
	bola.tipo = tipo;
	bola.dim = dim;
	bola.spd = spd;
	bola.img = img;
	bola.ativo = true;
	return bola;
}

BLOCO createBloco(VETOR2D pos, int tipo, int w, int h, int vida, SDL_Surface* img){
	BLOCO bloco;
	bloco.pos = pos;
	bloco.tipo = tipo;
	bloco.w = w;
	bloco.h = h;
	bloco.maxVida = vida;
	bloco.vida = bloco.maxVida;
	bloco.img = img;
	return bloco;
}

PLATAFORMA createPlataforma(VETOR2D pos, VETOR2D step, SDL_Surface* img){
	PLATAFORMA plat;
	plat.pos = pos;
	plat.dir = step;
	plat.w = 96;
	plat.h = 12;
	plat.img = img;
	plat.spd = 0;
	plat.ativo = true;
	return plat;
}

PWP createPwp(VETOR2D pos, VETOR2D dir, int tipo, double spd, int ativo, SDL_Surface* img){
	PWP pwp;
	pwp.pos = pos;
	pwp.dir = dir;
	pwp.tipo = tipo;
	pwp.img = img;
	pwp.spd = 0;
	pwp.ativo = ativo;
	return pwp;
}

int createNPCs() {
	VETOR2D pos, dir;
	int i;

	gPlayer.pontos = 0;
	gPlayer.vidas = 3;

	gBolas = calloc(MAX_NUM_BOLAS, sizeof(BOLA));
	if (!gBolas) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		return false;
	}
	gBlocos = calloc(BLOCKS_W*BLOCKS_H, sizeof(BLOCO));
	if (!gBlocos) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		return false;
	}
	gPad = calloc(1, sizeof(PLATAFORMA));
	if (!gPad) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		return false;
	}
	/*gPowerUp = calloc(1, sizeof(PWP));
	if (!gPad) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		return false;
	}*/

	pos.x = 300;
	pos.y = 300;

	gPowerUp = createPwp(pos, dir, 0, 10, 1, gPWPImgs[0]);

	for (i = 0; i < gNumBolas; i++) {
		pos.x = (rand() % (gGameWidth-64))+32;
		pos.y = (rand() % (gGameHeight-64))+32;
		dir.x = (rand() % 2? -1 : 1);
		dir.y = (rand() % 2? -1 : 1);
		gBolas[i] = createBola(pos, dir, 1, 10, gGameHeight/5, gBallImgs[0]);
	}

	pos.x = gGameWidth/2;
	pos.y = gGameHeight-56;
	dir.x = 4;
	dir.y = 4;

	gPad[0] = createPlataforma(pos, dir, gPadImgs[0]);

	return true;
}

void exitGame() {

	static int hasRan = 0;
	if (hasRan) return;

	SDL_FreeSurface(gScoreSurface);

	SDL_FreeSurface(gBallImgs[0]);
	SDL_FreeSurface(gBlocoBreak);
	SDL_FreeSurface(gPadImgs[0]);
	/*SDL_FreeSurface(gScoreSurface);*/

	gBallImgs[0] = NULL;
	gPadImgs[0] = NULL;

	if(!setClipboard()){
		printf("Erro ao renderizar clipboard.Verificar função setClipboard.\n");
	}

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	/*Mix_FreeChunk(gSons[0]);
	gSons[0] = NULL;*/
	Mix_CloseAudio();

	TTF_CloseFont(gScoreFonte);/*	encerra a utilização da fonte do ttf	*/

	TTF_Quit();/*	fecha o SDL_ttf	*/
	IMG_Quit();
	SDL_Quit();

	hasRan = true;
}

int handleInput(SDL_Event* evt){
	SDL_Event e = *evt;
	int quit = false;
	int i;
	switch (e.type) {
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_LEFT) {
				gLeft = true;
			}
			if (e.key.keysym.sym == SDLK_RIGHT) {
				gRight = true;
			}
			if (e.key.keysym.sym == SDLK_UP) {
				for (i=0; i<gNumBolas; i++)
				gBolas[i].ativo = false;
			}
			if (e.key.keysym.sym == SDLK_DOWN) {
				for (i=0; i<gNumBolas; i++)
				gBolas[i].ativo = true;
			}
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				quit = true;
			}
			break;
		case SDL_KEYUP:
			if (e.key.keysym.sym == SDLK_LEFT) {
				gLeft = false;
			}
			if (e.key.keysym.sym == SDLK_RIGHT) {
				gRight = false;
			}
			break;
		case SDL_MOUSEMOTION:
			SDL_GetMouseState(&gXMouse,&gYMouse);
			break;
	}
	return quit;
}

int handleEvent(SDL_Event* evt) {
	int quit = false;
	SDL_Event e = *evt;

	switch (e.type) {
		case SDL_QUIT:
			quit = true;
		break;
		default:
			quit = handleInput(evt);
	}

	return quit;
}

int collBallPlat(BOLA* a, double delta){
	double dx, dy, raio;
	VETOR2D c;
	int inv = 0;
	c.x = a->pos.x + a->dim/2.0;
	c.y = a->pos.y + a->dim/2.0;
	raio = a->dim/2.0;
	if (!isInAABB(c, gPad->pos.x - raio,
						  gPad->pos.y - raio,
						  gPad->pos.x + gPad->w + raio,
						  gPad->pos.y + gPad->h)) {
		return false;
	}
	if (isInAABB(c, gPad->pos.x,
					gPad->pos.y - raio,
					gPad->pos.x + gPad->w,
					gPad->pos.y + gPad->h)) {
		a->dir.y = -a->dir.y;
		a->pos.y += a->dir.y*a->spd*delta;
		return true;
	}
	else {
		dx = c.x - gPad->pos.x;
		dy = c.y - gPad->pos.y;
		if (a->dim*a->dim < (dx*dx)+(dy*dy)) {
			inv = 1;
		}
		dx += gPad->w;
		if (a->dim*a->dim < (dx*dx)+(dy*dy)) {
			inv = 1;
		}
		if (inv) {
			a->dir.x = -a->dir.x;
			a->dir.y = -a->dir.y;
			a->pos.x += a->dir.x*a->spd*delta;
			a->pos.y += a->dir.y*a->spd*delta;
			return true;
		}
	}
	return false;
}

int collBallBlock(BOLA* a, BLOCO* b, double delta) {
	double dx, dy;
	VETOR2D c;
	int inv;
	
	c.x = a->pos.x + a->dim/2.0;
	c.y = a->pos.y + a->dim/2.0;
	if (!isInAABB(c, b->pos.x - a->dim,
					 b->pos.y - a->dim,
					 b->pos.x + b->w + a->dim,
					 b->pos.y + b->h + a->dim) || isInside(c, b)) {
		return false;
	}

	if (isInAABB(c, b->pos.x - a->dim/2.0,
					b->pos.y,
					b->pos.x + b->w + a->dim/2.0,
					b->pos.y + b->h)) {
		printf("\npx=%.3lf, py=%.3lf\n", a->prevPos.x, a->prevPos.y);
		printf("ox=%.3lf, oy=%.3lf\n", a->pos.x, a->pos.y);
						
		a->dir.x = (c.x > b->pos.x + tol)? fabs(a->dir.x) : -fabs(a->dir.x) ;
		a->pos.x = a->prevPos.x;
		a->pos.y = a->prevPos.y;
		moveBall(a, delta);
		printf("Primeiro if ");

	}
	else if (isInAABB(c, b->pos.x,
						 b->pos.y - a->dim/2.0,
						 b->pos.x + b->w,
						 b->pos.y + b->h + a->dim/2.0)) {
		 
		printf("\npx=%.3lf, py=%.3lf\n", a->prevPos.x, a->prevPos.y);
		printf("ox=%.3lf, oy=%.3lf\n", a->pos.x, a->pos.y);
		
		a->dir.y = (c.y > b->pos.y + tol)? fabs(a->dir.y) : -fabs(a->dir.y) ;
		a->pos.y = a->prevPos.y;
		a->pos.x = a->prevPos.x;
		moveBall(a, delta);
		printf("Segundo if ");

	}
	else {
		dx = c.x - b->pos.x;
		dy = c.y - b->pos.y;
		inv = collBallPoint(a, dx, dy, delta);
		dx += b->w;
		if (!inv) {
			inv = collBallPoint(a, dx, dy, delta);
		}
		dy += b->h;
		if (!inv) {
			inv = collBallPoint(a, dx, dy, delta);
		}
		dx -= b->w;
		if (!inv) {
			inv = collBallPoint(a, dx, dy, delta);
		}
		if (!inv) return false;
	}
	printf("Vida --: px=%.8lf py=%.8lf v=%.8lf\n", a->pos.x, a->pos.y, a->spd*delta);
	if (b->vida-- == 1) {
        puts("E morreu. o que houve?");
    }

	return true;
}

int collBallPoint(BOLA* a, double dx, double dy, double delta) {
	if ( (a->dim /2.0 )* (a->dim / 2.0) + 4*tol > (dx*dx)+(dy*dy)) {
		if (dx < dy) {
			a->dir.y = -a->dir.y;
			a->pos.x = a->prevPos.x;
			a->pos.y = a->prevPos.y;
			moveBall(a, delta);
		}
		else if (dx > dy) {			
			a->dir.x = -a->dir.x;
			a->pos.x = a->prevPos.x;
			a->pos.y = a->prevPos.y;
			moveBall(a, delta);
		}
		else {

			a->dir.x = -a->dir.x;
 			a->dir.y = -a->dir.y;
			a->pos.x = a->prevPos.x;
			a->pos.y = a->prevPos.y;
			moveBall(a, delta);
		}
		return true;
	}
	return false;
}
