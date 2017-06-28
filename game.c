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
			gGameStatus = -2;
		return false;
	}

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
		fprintf(stderr, "Erro: SDL_mixer nao conseguiu inicializar: %s\n",
			Mix_GetError());
			gGameStatus = -3;
		return false;
	}

	if(TTF_Init() < 0){
		fprintf(stderr,"Erro: SDL_ttf nao conseguiu inicializar: %s\n",
			TTF_GetError());
		gGameStatus = -4;
		return false;
	}

	Mix_AllocateChannels(8); /* Aloca 8 canais de audio pro jogo, sem contar musicas */

	gWindow = SDL_CreateWindow("Breakout",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		gScreenWidth, gScreenHeight,
		SDL_WINDOW_SHOWN);

	if (!gWindow) {
		fprintf(stderr, "Erro: Janela nao pode ser criada: %s\n",
			SDL_GetError());
		gGameStatus = -5;
		return false;
	}

	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fprintf(stderr, "Erro: SDL_image nao conseguiu inicializar: %s\n",
			SDL_GetError());
		gGameStatus = -6;
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

	if (p->pos.x + p->dim > gGameWidth-OFFSET || p->pos.x < OFFSET) {
		p->dir.x = -p->dir.x;
		p->pos.x = p->prevPos.x;
		Mix_PlayChannel(-1, gSons[SOUND_WALL], 0);
	}
	if (p->pos.y + p->dim > gGameHeight-OFFSET) {
		/*jogador perde pontos*/
		Mix_PlayChannel(-1, gSons[SOUND_LIFE_LOST], 0);
		gPlayer.pontos -= 1000;
        
		if(gPlayer.pontos < 0){
			gPlayer.pontos = 0;
		}
        
        /* Ultima bola(cha) alive */
        if (gNumBolasAlive-- == 1) {
            
            gPlayer.vidas--;
            
            gNumBolasAlive = 1;
            
            /*cola a bola*/
            p->colada=true;
            p->spd = gGameHeight/2.5; /* reseta a vel. */
            p->pos.x = gPad[0].pos.x + gPad[0].w/2 - p->dim/2;
            p->pos.y = gPad[0].pos.y - p->dim;
            p->dir.x = (rand() % 2? -1 : 1);
            p->dir.y = -1;
            normalize(&p->dir);
        }
        else {
            p->ativo = false;
        }

		/* Mix_PlayChannel(-1, gSons[SOUND_FLOOR], 0); */
	}
	else if (p->pos.y < 32) {
		p->dir.y = -p->dir.y;
		p->pos.y  = p->prevPos.y;
		Mix_PlayChannel(-1, gSons[SOUND_WALL], 0);
	}
	p->lastDelta = delta;
}

void movePwp(PWP* p, double delta){

	if (p->ativo){
		p->prevPos.x = p->pos.x;
		p->prevPos.y = p->pos.y;

		p->dir.y += 0.0075;

		p->pos.x += p->dir.x*p->spd*delta;
		p->pos.y += p->dir.y*p->spd*delta;
	}

	if (p->pos.x + p->dim > gGameWidth-OFFSET) {
		p->dir.x = -p->dir.x;
		p->pos.x = gGameWidth-OFFSET-p->dim;
		Mix_PlayChannel(-1, gSons[SOUND_WALL], 0);
	}
	if (p->pos.x < OFFSET) {
		p->dir.x = -p->dir.x;
		p->pos.x = OFFSET;
		Mix_PlayChannel(-1, gSons[SOUND_WALL], 0);
	}
	if (p->pos.y + p->dim > gGameHeight-OFFSET) {
		p->ativo = 0;
	}
	else if (p->pos.y < 32) {
		p->dir.y = -p->dir.y;
		p->pos.y  = p->prevPos.y;
		Mix_PlayChannel(-1, gSons[SOUND_WALL], 0);
	}
	p->lastDelta = delta;

}


void movePlataforma (PLATAFORMA* p, double delta) {
	int i;
	double dx, origX = p->pos.x;

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
	else if (p->spd < 0.5 && p->spd > -0.5) p->spd = 0;

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

	/* movimento das bolas coladas */
	dx = p->pos.x - origX;

	for (i = 0; i < gNumBolas; i++) {
		if (gBolas[i].colada) {
			gBolas[i].pos.x += dx;
		}
	}

}

int gameLoop(double delta) {
	int i, j, spdUp = false;

	if (gNumBlocosAlive < 1) {
		goToNextLevel();
	}

	movePlataforma(gPad, delta);

	movePwp(&gPowerUp, delta);
	if (gPowerUp.ativo && collPwpPlat(&gPowerUp, gPad, delta)){

		gPowerUp.ativo = false;

		switch(gPowerUp.tipo){
			case 0: gPlayer.vidas--; Mix_PlayChannel(-1, gSons[SOUND_LIFE_LOST], 0); break;
			case 1: if (++gPlayer.vidas > MAXVIDAS) gPlayer.vidas = 4; break;
			case 2: dupaBalls(); break;
			case 3: spdUp = true; break;
			case 4: printf("Mano parabens isso era pra aumentar a plataforma\n"); break;
			case 5: printf("Mano parabens isso era pra diminuir a plataforma\n"); break;
			case 6: gPad->dir.x /= 1.5; break;
			case 7: gPad->dir.x *= 1.5; break;
		}

	}

	for (i = 0; i < gNumBolas; i++) {
		if (gBolas[i].ativo){
			
			if (spdUp){
				/* printf("spd antes: %f\n", gBolas[i].spd); */
				gBolas[i].spd *= 1.5;
				/* printf("spd depois: %f\n", gBolas[i].spd); */
			}

			if (gBolas[i].colada) continue; /* o movimento dela eh no movePlataforma */

            moveBall(gBolas+i, delta);
			if (collBallPlat(gBolas+i, delta)) {
                platModify(gBolas+i);
				Mix_PlayChannel(-1, gSons[SOUND_PLAT], 0);
			}

			for (j = 0; j < gNumBlocos; j++) {
				if (gBlocos[j].vida) {
					if (collBallBlock(gBolas+i, gBlocos+j, delta)) {
						gPlayer.pontos += 100;
						gAllPts += 100;
						if (gPlayer.pontos > 9999999) gPlayer.pontos = 9999999; /* menos segfault, mais alegria */
						if(gAllPts % 10000 == 0)
						{
							if (++gPlayer.vidas > MAXVIDAS) gPlayer.vidas = 4;
						}
					}
				}
			}
		}
		if(gPlayer.vidas < 0) {
			gGameStatus = 101;
			gGameOver = true;
			SDL_UpdateWindowSurface(gWindow);
			return true;
		}
	}
    spdUp = false;
    
	return false;
}

BOLA createBola(VETOR2D pos, VETOR2D step, int tipo, int dim, double spd, SDL_Surface* img) {
	BOLA bola;
	bola.pos = pos;
	bola.prevPos = copyVector(pos);
	bola.lastDelta = 0;
	bola.dir = step;
	bola.tipo = tipo;
	bola.dim = dim;
	bola.spd = spd;
	bola.img = img;
	bola.ativo = true;
	bola.colada = true;
	return bola;
}

BOLA cloneBola(BOLA* orig) {
	BOLA bola;
	bola.pos = copyVector(orig->pos);
	bola.prevPos = copyVector(orig->prevPos);
	bola.lastDelta = orig->lastDelta;
	bola.dir = orig->dir;
	bola.tipo = orig->tipo;
	bola.dim = orig->dim;
	bola.spd = orig->spd;
	bola.img = orig->img;
	bola.ativo = orig->ativo;
	bola.colada = orig->colada;
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

PWP createPwp(VETOR2D pos, VETOR2D dir, int tipo, int dim, double spd, int ativo, SDL_Surface* img){
	PWP pwp;
	pwp.pos = pos;
	pwp.prevPos = copyVector(pos);
	pwp.lastDelta = 0;
	pwp.dir = dir;
	pwp.tipo = tipo;
	pwp.dim = dim;
	pwp.spd = spd;
	pwp.ativo = ativo;
	pwp.img = img;

	return pwp;
}

int createNPCs() {
	VETOR2D pos, dir;
	int i;

	gNumBolas = 1;
    gNumBolasAlive = 1;

	if (gBolas) free(gBolas);
	gBolas = calloc(MAX_NUM_BOLAS, sizeof(BOLA));
	if (!gBolas) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		gGameStatus = -abs(errno);
		return false;
	}
	if (gBlocos) free(gBlocos);
	gBlocos = calloc(BLOCKS_W*BLOCKS_H, sizeof(BLOCO));
	if (!gBlocos) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		gGameStatus = -abs(errno);
		return false;
	}
	if (gPad) free(gPad);
	gPad = calloc(1, sizeof(PLATAFORMA));
	if (!gPad) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		gGameStatus = -abs(errno);
		return false;
	}

	pos.x = 800;
	pos.y = 40;

	dir.x = -1;
	dir.y = 1;

	gPowerUp = createPwp(pos, dir, 0, 24, 50, 1, gPWPImgs[0]);

	for (i = 0; i < gNumBolas; i++) {
		pos.x = gGameWidth/2 - gBallImgs[0]->w/2;
		pos.y = gGameHeight-56 - gBallImgs[0]->h;
		dir.x = (rand() % 2? -1 : 1);
		dir.y = -1;
		normalize(&dir);
		gBolas[i] = createBola(pos, dir, 1, 10, gGameHeight/2.5, gBallImgs[0]);
		if (!i){
			gBolas[i].ativo = true;
		}else{
			gBolas[i].ativo = false;
		}
	}

	pos.x = gGameWidth/2 - gPadImgs[0]->w/2;
	pos.y = gGameHeight-56;
	dir.x = 4;
	dir.y = 4;

	gPad[0] = createPlataforma(pos, dir, gPadImgs[0]);

	return true;
}

void exitGame() {

	int i;

	static int hasRan = 0;
	if (hasRan) return;

	SDL_FreeSurface(gScoreSurface);
	SDL_FreeSurface(gBallImgs[0]);

	for(i = 0; i < 10; i++) {
		SDL_FreeSurface(gBlocoImgs[i]);
	}
	for(i = 0; i < 2; i++) {
		SDL_FreeSurface(gLed[i]);
	}
	for(i = 0; i < 4; i++) {
		SDL_FreeSurface(gPWPImgs[i]);
	}
	SDL_FreeSurface(gBlocoBreak);
	SDL_FreeSurface(gPadImgs[0]);
	/*SDL_FreeSurface(gScoreSurface);*/

	gBallImgs[0] = NULL;
	gPadImgs[0] = NULL;

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
            /*REMOVER*/
			if (e.key.keysym.sym == SDLK_k) {
				gNumBlocosAlive = 0; /* fuk da police */
			}
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
			if (e.key.keysym.sym == SDLK_SPACE) {
				for (i=0; i<gNumBolas; i++) {
					if (gBolas[i].colada) {
						gBolas[i].colada = false;
						break;
					}
				}
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
			gGameStatus = 200;
			/* quit = true; */
            exit(0);
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

	if (a->dir.y < 0 || a->colada) { /* bola ta indo pra cima ou ta colada, n colide! */
		return false;
	}

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
			return true;
		}
	}
	return false;
}

int collPwpPlat(PWP* p, PLATAFORMA* b, double delta){
	VETOR2D p1, p2;

	p1.x = p->pos.x;
	p1.y = p->pos.y + p->dim;

	p2.x = p->pos.x + p->dim;
	p2.y = p->pos.y + p->dim;

	if (isInAABB(p1, b->pos.x, 			b->pos.y,
					 b->pos.x + b->w, 	b->pos.y + b->h)
		||  isInAABB(p2, b->pos.x, 		b->pos.y,
					 b->pos.x + b->w, 	b->pos.y + b->h)) {
	   return true;
   }
	return false;

}

int collBallBlock(BOLA* a, BLOCO* b, double delta) {
	double dx, dy;
	VETOR2D c;
	int inv;

	c.x = a->pos.x + a->dim/2.0;
	c.y = a->pos.y + a->dim/2.0;
	if (!isInAABB(c, b->pos.x - a->dim/2.0,
					 b->pos.y - a->dim/2.0,
					 b->pos.x + b->w + a->dim/2.0,
					 b->pos.y + b->h + a->dim/2.0) || isInside(c, b)) {
		return false;
	}

	if (isInAABB(c, b->pos.x - a->dim/2.0,
					b->pos.y,
					b->pos.x + b->w + a->dim/2.0,
					b->pos.y + b->h)) {

		a->dir.x = (c.x > b->pos.x + tol)? fabs(a->dir.x) : -fabs(a->dir.x) ;
		if (a->dir.x > 0) {
			a->pos.x = b->pos.x+b->w+0.125;
		}
		else {
			a->pos.x = b->pos.x - a->dim-0.125;
		}
		a->pos.y = a->prevPos.y;
		/* printf("Primeiro if "); */

	}
	else if (isInAABB(c, b->pos.x,
						 b->pos.y - a->dim/2.0,
						 b->pos.x + b->w,
						 b->pos.y + b->h + a->dim/2.0)) {

		a->dir.y = (c.y > b->pos.y + tol)? fabs(a->dir.y) : -fabs(a->dir.y) ;
		if (a->dir.y > 0) {
			a->pos.y = b->pos.y+b->h+0.125;
		}
		else {
			a->pos.y = b->pos.y - a->dim-0.125;
		}
		a->pos.x = a->prevPos.x;
		/* printf("Segundo if "); */

	}
	else {
		dx = b->pos.x - c.x;
		dy = b->pos.y - c.y;
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
	/* printf("Vida --: px=%.8f py=%.8f v=%.8f\n", a->pos.x, a->pos.y, a->spd*delta); */
	if (b->vida-- == 1) {
        Mix_PlayChannel(-1, gSons[SOUND_BLOCK_BROKE], 0);

		if (rand()%4 == 0){
			if (!gPowerUp.ativo){
				int jooj = rand()%8;
				gPowerUp.ativo = true;
				gPowerUp.img = gPWPImgs[jooj];
				gPowerUp.tipo = jooj;
				gPowerUp.pos.x = a->pos.x;
				gPowerUp.pos.y = a->pos.y;
				gPowerUp.dir.x = 5*a->dir.x;
				gPowerUp.dir.y = -2*a->dir.y;
			}
		}

		if (b->tipo == 7) {
			destroiVizinhos(b);
		}
        gNumBlocosAlive--;

    }

	return true;
}

void destroiVizinhos(BLOCO* b){
	int i;
	VETOR2D point;
	Mix_PlayChannel(-1, gSons[SOUND_EXPLODE], 0);
	b->vida = 0;
	for(i = 0; i < gNumBlocos; i++){
		
		if (gBlocos[i].vida){
			
			point.x = b->pos.x + BLOCK_DIST + b->w;
			point.y = b->pos.y;
					
			if(gBlocos[i].pos.x == point.x
			&& gBlocos[i].pos.y == point.y){
				
				gBlocos[i].vida = 0;
				if (gBlocos[i].tipo == 7){
					destroiVizinhos(&gBlocos[i]);
				}		
			}
			
			point.x = b->pos.x;
			point.y = b->pos.y - BLOCK_DIST - b->h;
			
			if(gBlocos[i].pos.x == point.x
			&& gBlocos[i].pos.y == point.y){
				
				gBlocos[i].vida = 0;
				if (gBlocos[i].tipo == 7){
					destroiVizinhos(&gBlocos[i]);
				}		
			}
			
			point.x = b->pos.x - BLOCK_DIST - b->w;
			point.y = b->pos.y;
			
			if(gBlocos[i].pos.x == point.x
			&& gBlocos[i].pos.y == point.y){
				
				gBlocos[i].vida = 0;
				if (gBlocos[i].tipo == 7){
					destroiVizinhos(&gBlocos[i]);
				}		
			}
			
			point.x = b->pos.x;
			point.y = b->pos.y + BLOCK_DIST + b->h;
			
			if(gBlocos[i].pos.x == point.x
			&& gBlocos[i].pos.y == point.y){
				
				gBlocos[i].vida = 0;
				if (gBlocos[i].tipo == 7){
					destroiVizinhos(&gBlocos[i]);
				}		
			}
		}	
	}
}


int collBallPoint(BOLA* a, double dx, double dy, double delta) {
	if ( (a->dim /2.0 )* (a->dim / 2.0) > (dx*dx)+(dy*dy)) {
		dx = fabs(dx);
		dy = fabs(dy);
		if (dx < dy) {
			a->dir.y = -a->dir.y;
			a->pos.y += a->dir.y*a->spd*a->lastDelta;
		}
		else if (dx > dy) {
			a->dir.x = -a->dir.x;
			a->pos.x += a->dir.x*a->spd*a->lastDelta;
		}
		else {

			a->dir.x = -a->dir.x;
 			a->dir.y = -a->dir.y;

			a->pos.x += a->dir.x*a->spd*a->lastDelta;
			a->pos.y += a->dir.y*a->spd*a->lastDelta;

		}
		return true;
	}
	return false;
}

int goToNextLevel() {

	if (gLvlNumber > 0) {
		gPlayer.pontos += 10000;
		gAllPts += 10000;
		if (gPlayer.pontos > 9999999) gPlayer.pontos = 9999999; /* menos segfault, mais alegria */
		if (++gPlayer.vidas > MAXVIDAS) gPlayer.vidas = 4;
	}
	else {
		gPlayer.pontos = 0;
		gPlayer.vidas = 3;
		gAllPts = 0;
	}

	createNPCs();
	loadBlocosFromNumber(gLvlNumber++ % 9);

	return true;
}


void platModify(BOLA* b) {
    int sig;
    double bx = b->pos.x + b->dim/2.0;
    double dx = bx - (gPad->pos.x + gPad->w/2.0);

    sig =  (dx > 0)? 1 : -1;
    dx = fabs(dx);
    
    /* dx -= gPad->w/3.0;
       dx /= gPad->w/2.0;
       dx *= GAME_PI/6; */
    
    dx = GAME_PI*(dx - gPad->w/3.0)/(gPad->w * 3.0);

    b->dir.x = 1;
    b->dir.y = -1;

    turnRad(&b->dir, dx);
    normalize(&b->dir);
    b->dir.x *= sig;
}

void dupaBalls() {
    int i, j, oNumBalls = gNumBolas;
    
    for (i = 0; i < oNumBalls; i++) {
        if (gNumBolas == MAX_NUM_BOLAS) {
            break;
        }
        
        if (gBolas[i].ativo != true) continue;
        
        for (j = 0; j <= oNumBalls; j++) {
            if (j == oNumBalls) {
                gBolas[gNumBolas++] = cloneBola(gBolas+i);
                gNumBolasAlive++;
                gBolas[gNumBolas-1].dir.x = -gBolas[i].dir.x;
                gBolas[gNumBolas-1].ativo = 2; /* Nao passa pelo filtro acima, evitando duplicar bolas recem-criadas */
                break;
            }
            else if (gBolas[j].ativo == false) {
                /* Opaa reativamos */
                gBolas[j] = cloneBola(gBolas+i);
                gNumBolasAlive++;
                gBolas[j].dir.x = -gBolas[i].dir.x;
                gBolas[j].ativo = 2; /* Nao passa pelo filtro acima, evitando duplicar bolas recem-criadas */
                break;
            }
        }
    }
    /* Limpa filtro */
    for (i = 0; i < gNumBolas; i++) {
        if (gBolas[i].ativo > 1) gBolas[i].ativo = true;
    }
}
