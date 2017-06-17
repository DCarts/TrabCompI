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

	p->pos.x = p->pos.x + p->dir.x*p->spd*delta;
	p->pos.y = p->pos.y + p->dir.y*p->spd*delta;

	if (p->pos.x + p->dim > gScreenWidth-OFFSET - gScoreBoardWidth || p->pos.x < OFFSET) {
		p->dir.x = -p->dir.x;
		p->pos.x += p->dir.x*p->spd*delta;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], 0); */
	}
	if (p->pos.y + p->dim > gScreenHeight-OFFSET) {
		/*jogador perde pontos*/
		gPlayer.pontos -= 1000;
		if(gPlayer.pontos < 0){	gPlayer.pontos = 0;}
		
		p->ativo = false;
		p->dir.y = -p->dir.y;
		p->pos.y += p->dir.y*p->spd*delta;
		/* Mix_PlayChannel(-1, gSons[SOUND_FLOOR], 0); */
	}
	else if (p->pos.y < 32) {
		p->dir.y = -p->dir.y;
		p->pos.y += p->dir.y*p->spd*delta;
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



	if (p->pos.x + p->w > gScreenWidth-OFFSET - gScoreBoardWidth) {
		p->spd = 0;
		p->pos.x = gScreenWidth-OFFSET-p->w - gScoreBoardWidth;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], 0); */ // nao seria uma boa ideia colocar o som de colisao da bolinha pra plataforma
	}

	if (p->pos.x < OFFSET) {
		p->spd = 0;
		p->pos.x = OFFSET;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], 0); */ //idem
	}
}

/* @todo fazer funcionar direito */
void collide(BOLA* a, BOLA* b, double delta) {
	/*VETOR2D c, mp, n, tip, ray;
	double dot;*/

	/* bola n precisa colidir mesmo */


}

int gameLoop(double delta) {
	int i, j;

	movePlataforma(gPad, delta);

	for (i = 0; i < gNumBolas; i++) {
		if (gBolas[i].ativo){

			for (j = 0; j < gNumBlocos; j++) {
				if (gBlocos[j].vida) {
					if (collBallBlock(gBolas+i, gBlocos+j, delta)) {
						gPlayer.pontos += 100;
					}
				}
			}

			moveBall(gBolas+i, delta);
			collBallPlat(gBolas+i, delta);
			//detCollBlocos(alalalal
		}
	}
	/* colisao entre bolas */
	/*for (i = 0; i < gNumBolas; i++) {
		for (j = i+1; j < gNumBolas; j++) {

			sqDistance = sqDist(gBolas[i].pos, gBolas[j].pos);
			raios = (gBolas[i].dim/2) + (gBolas[j].dim/2);

			if (raios*raios >= sqDistance) {
				collide(gBolas+i, gBolas+j, delta);
			}
		}
	}*/

	return false;
}

BOLA createBola(VETOR2D pos, VETOR2D step, int tipo, int dim, double spd, SDL_Surface* img) {
	BOLA bola;
	bola.pos = pos;
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

int createNPCs() {
	VETOR2D pos, dir;
	int i;


	gPlayer.ativo = false;
	while(gPlayer.ativo == false){
		createPlayer();
	}

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

	for (i = 0; i < gNumBolas; i++) {
		pos.x = (rand() % (gScreenWidth-64))+32;
		pos.y = (rand() % (gScreenHeight-64))+32;
		dir.x = (rand() % 2? -1 : 1);
		dir.y = (rand() % 2? -1 : 1);
		gBolas[i] = createBola(pos, dir, 1, 10, gScreenHeight/5, gBallImgs[0]);
	}

	pos.x = gScreenWidth/2;
	pos.y = gScreenHeight-56;
	dir.x = 4;
	dir.y = 4;

	gPad[0] = createPlataforma(pos, dir, gPadImgs[0]);

	return true;
}

void exitGame() {
	TTF_CloseFont(gFonte);/*	encerra a utilização da fonte ttf	*/

	SDL_FreeSurface(gBallImgs[0]);
	SDL_FreeSurface(gBlocoBreak);
	SDL_FreeSurface(gPadImgs[0]);
	/*SDL_FreeSurface(gTexto);*/

	gBallImgs[0] = NULL;
	gPadImgs[0] = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	/*Mix_FreeChunk(gSons[0]);
	gSons[0] = NULL;*/
	Mix_CloseAudio();
	TTF_Quit();/*	fecha o SDL_ttf	*/
	IMG_Quit();
	SDL_Quit();
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
	c.x = a->pos.x + a->dim/2;
	c.y = a->pos.y + a->dim/2;
	raio = a->dim/2;
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
	c.x = a->pos.x + a->dim/2;
	c.y = a->pos.y + a->dim/2;
	if (!isInAABB(c, b->pos.x - a->dim/2,
					 b->pos.y - a->dim/2,
					 b->pos.x + b->w + a->dim/2,
					 b->pos.y + b->h + a->dim/2)) {
		return false;
	}

	if (isInAABB(c, b->pos.x,
					 b->pos.y,
					 b->pos.x + b->w,
					 b->pos.y + b->h)) {

		a->dir.x = -a->dir.x;
		a->dir.y = -a->dir.y;
		a->pos.x += a->dir.x*a->spd*delta;
		a->pos.y += a->dir.y*a->spd*delta;

		return false;
	}
	else if (isInAABB(c, b->pos.x - a->dim/2,
					b->pos.y,
					b->pos.x + b->w + a->dim/2,
					b->pos.y + b->h)) {
		a->dir.x = -a->dir.x;
		a->pos.x += a->dir.x*a->spd*delta;

	}
	else if (isInAABB(c, b->pos.x,
						 b->pos.y - a->dim/2,
						 b->pos.x + b->w,
						 b->pos.y + b->h + a->dim/2)) {
		a->dir.y = -a->dir.y;
		a->pos.y += a->dir.y*a->spd*delta;
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
	printf("Vida --: bx=%.2lf by=%.2lf\n", a->pos.x+dx, a->pos.y+dy);
	b->vida--;

	return true;
}

int collBallPoint(BOLA* a, double dx, double dy, double delta) {
	if ( (a->dim /2 )* (a->dim / 2) > (dx*dx)+(dy*dy)) {
		printf("1: bx=%.2lf by=%.2lf\n", a->pos.x+dx, a->pos.y+dy);
		if (dx < dy) {
			a->dir.x = -a->dir.x;
			a->pos.x += a->dir.x*a->spd*delta;
		}
		else if (dx > dy) {
			a->dir.y = -a->dir.y;
			a->pos.y += a->dir.y*a->spd*delta;
		}
		else {

			a->dir.x = -a->dir.x;
			a->dir.y = -a->dir.y;
			a->pos.x += a->dir.x*a->spd*delta;
			a->pos.y += a->dir.y*a->spd*delta;
		}
		printf("2: bx=%.2lf by=%.2lf\n", a->pos.x+dx, a->pos.y+dy);
		return true;
	}
	return false;
}


void createPlayer(){
	char buffer[22];

	while(true){	/*permanecer na função até que o usuário nao digite merda(digite algum caracter)*/
		printf("Qual o nome do jogador? \n");
		gets(buffer);

		if (sscanf(buffer, "%s",gPlayer.nome) != EOF) {
			strcpy(gPlayer.nome, buffer);
			break;
		}
	}

		/*gPlayer.nome[strlen(gPlayer.nome) - 1] = '\0';*/

		/*	Captura a altura e a largura do texto TTF
		if (TTF_SizeText(gFonte,gPlayer.nome,&gScoreBoardWidth,
											&gScoreBoardHeight) == -1){
												fprintf(stderr,
												"Erro capturando as dimensões da fonte! %s\n",
												TTF_GetError());
											}
		else{
			printf("width = %d \t height = %d\n",gScoreBoardWidth,gScoreBoardHeight);
		}
		*/


		gPlayer.vidas = 2;
		gPlayer.pontos = 0;

		gPlayer.ativo = true;
}
