/*
 * main.c
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 * 
 * 		AS VELOCIDADES SÃO POR SEGUNDO
 * 		Px / Segundo
 */

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <errno.h>

/* Constantes */
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

/* Tipos de bloco/bola */

/* Structs */
typedef struct _VETOR2D {
	double x;
	double y;
} VETOR2D;

typedef struct _BOLA {
	VETOR2D pos;
	VETOR2D dir;
	int tipo;
	int ativo;
	int dim;
	double spd;
	SDL_Surface* img;
} BOLA;

typedef struct _PLATAFORMA {
	VETOR2D pos;
	VETOR2D dir;
	int melhorias[5];
	int ativo;
	int w;
	int h;
	double spd;
	SDL_Surface* img;
} PLATAFORMA;

typedef struct _BLOCO {
	VETOR2D pos;
	int tipo;
	int w, h;
	int vida;
	SDL_Surface* img;
} BLOCO;

/* Variaveis Globais */

int gScreenWidth = 640;
int gScreenHeight = 480;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gBlocoImgs[10];
SDL_Surface* gBallImgs[5];
SDL_Surface* gPadImgs[5];

Mix_Chunk* gSons[10];

int gNumBolas = 6;
int gNumBlocos = 0;

PLATAFORMA* gPad;
BOLA* gBolas;
BLOCO* gBlocos;

int gLeft, gRight;

int gXMouse, gYMouse;

/* Funcoes da main.c */

/* Inicia o jogo */
int init();

/* Carrega as imagens e audio */
int loadMedia();

/* Carrega uma imagem */
SDL_Surface* loadSurface (char* path);

/* Carrega um som */
Mix_Chunk* loadSound(char* path);

/* Cria os NPCs */
int createNPCs();

/* Cria uma bolinha */
BOLA createBola(VETOR2D pos, VETOR2D dir, int tipo, int dim, double spd, SDL_Surface* img);
/*Cria um bloco*/
BLOCO createBloco(VETOR2D pos, int tipo, int w, int h, SDL_Surface* img);
/*Cria a plataforma*/
PLATAFORMA createPlataforma(VETOR2D pos, VETOR2D dir, SDL_Surface* img);

/* Move uma bolinha */
void moveBall(BOLA* b, double delta);
/* Move a plataforma */
void movePlataforma(PLATAFORMA *p, double delta);

/* Loop do jogo */
int gameLoop();

/* Renderiza a tela */
int render();

/* Processa eventos */
int handleEvent(SDL_Event* e);
int handleInput(SDL_Event* evt);

/* Carrega blocos de um level *
 * O level deve estar em ./data/level/ *
 * Retorna true se leu de boas, false se deu m */
int loadBlocosFromFile(char* levelName);

/* Encerra os sistemas e sai do jogo */
void exitGame();

/* Funcoes de math uteis */
double normalize (VETOR2D* a);
double dotProduct(VETOR2D a, VETOR2D b);
double sqDist (VETOR2D a, VETOR2D b);
int isInAABB(VETOR2D t, double p1x, double p1y, double p4x, double p4y);

/* Funcoes de colisao */
/* Algumas sao mei chatas msm e ainda estao em construcao WIP */
void collide (BOLA* a, BOLA* b, double delta);
void collBallPlat(BOLA* a, double delta);
void collPlatBall(BOLA* a, double delta);
void collBallBlock(BOLA* a, BLOCO* b, double delta);
int collBallPoint(BOLA* a, double dx, double dy, double delta);

/* Fim das funcoes da main.c */

int main(int argc, char **argv) {

	int quit;
	long startTime, currentTime, lastTime;
	double delta;
	SDL_Event evt;

	if (!init()) {
		return 1;
	}
	
	atexit(exitGame);
	
	if (!loadMedia()) {
		return 1;
	}

	createNPCs();
	loadBlocosFromFile("teste");

	quit = false;
	currentTime = startTime = SDL_GetTicks();

	gLeft = false;
	gRight = false;

	/* Timer do jogo */
	while (!quit) {
		while (SDL_PollEvent(&evt) != 0) {
			quit = handleEvent(&evt);
		}
		lastTime = currentTime;
		currentTime = SDL_GetTicks();
		delta = (currentTime-lastTime)/1000.0;
		if (gameLoop(delta)) return 1;
		if (render()) return 1;
	}
	exitGame();
	return 0;
}

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

int loadMedia() {
	/* CARREGANDO IBAGENS */
	/* ColorKey eh magenta */
    uint32_t colorKey;
    int i;

    /* Carrega blocos *//* @todo
    if( !(gBlockImgs[0] = loadSurface("./data/brick.png")) ) return false;
    SDL_SetColorKey(gBlockImgs[0], SDL_TRUE, colorKey);*/

    /* Carrega plataformas */
    if( !(gPadImgs[0] = loadSurface("./data/pad.png")) ) return false;
    colorKey = SDL_MapRGB(gPadImgs[0]->format, 0xFF, 0x00, 0xFF );
    SDL_SetColorKey(gPadImgs[0], SDL_TRUE, colorKey);

    /* Carrega bolas */
    if ( !(gBallImgs[0] = loadSurface("./data/ball.png")) ) return false;
    colorKey = SDL_MapRGB(gBallImgs[0]->format, 0xFF, 0x00, 0xFF );
    SDL_SetColorKey(gBallImgs[0], SDL_TRUE, colorKey);
    /* FIM CARREGANDO IBAGENS */

    /* Carrega tijolos */
    if ( !(gBlocoImgs[0] = loadSurface("./data/brick0.png")) ) return false;
    if ( !(gBlocoImgs[1] = loadSurface("./data/brick1.png")) ) return false;
    if ( !(gBlocoImgs[2] = loadSurface("./data/brick2.png")) ) return false;
    if ( !(gBlocoImgs[3] = loadSurface("./data/brick3.png")) ) return false;
    if ( !(gBlocoImgs[4] = loadSurface("./data/brick4.png")) ) return false;
    if ( !(gBlocoImgs[5] = loadSurface("./data/brick5.png")) ) return false;
    if ( !(gBlocoImgs[6] = loadSurface("./data/brick6.png")) ) return false;
    if ( !(gBlocoImgs[7] = loadSurface("./data/brick7.png")) ) return false;
    if ( !(gBlocoImgs[8] = loadSurface("./data/brick8.png")) ) return false;
    if ( !(gBlocoImgs[9] = loadSurface("./data/brick9.png")) ) return false;

    for (i=0; i<10; i++){
		colorKey = SDL_MapRGB(gBlocoImgs[i]->format, 0xFF, 0x00, 0xFF );
		SDL_SetColorKey(gBlocoImgs[i], SDL_TRUE, colorKey);
	}

    /* CARREGANDO SONS */
    /* volume do som varia entre 0 e 127 */
    //if ( !(gSons[0] = loadSound("./data/wall.wav")) ) return false;
    //Mix_VolumeChunk(gSons[0], 64);
    /* FIM CARREGANDO SONS */
    return true;
}

Mix_Chunk* loadSound(char* path) {
	Mix_Chunk* sound = NULL;
	sound = Mix_LoadWAV(path);
	if (!sound) {
		fprintf(stderr, "Erro: incapaz de carregar som: %s\n%s\n",
						path, Mix_GetError() );
	}
	return sound;
}

SDL_Surface* loadSurface(char* path ) {
    /* A imagem otimizada final */
    SDL_Surface* optimizedSurface = NULL;

    /* Carrega imagem no PATH dado */
    SDL_Surface* loadedSurface = IMG_Load( path );
    if(!loadedSurface) {
        fprintf(stderr, "Erro: incapaz de carregar imagem: %s\n%s\n",
						path, IMG_GetError() );
    }
    else {
        /* Converte a imagem ao formato da tela */
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0 );
        if(!optimizedSurface) {
            fprintf(stderr, "Erro: incapaz de otimizar imagem: %s\n%s\n",
							path, SDL_GetError() );
        }

        /* Libera a imagem normal da memoria */
        SDL_FreeSurface( loadedSurface );
    }

    return optimizedSurface;
}

void moveBall(BOLA* p, double delta) {

	p->pos.x = p->pos.x + p->dir.x*p->spd*delta;
	p->pos.y = p->pos.y + p->dir.y*p->spd*delta;

	if (p->pos.x + p->dim > gScreenWidth-OFFSET || p->pos.x < OFFSET) {
		p->dir.x = -p->dir.x;
		p->pos.x += p->dir.x*p->spd*delta;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], 0); */
	}
	if (p->pos.y + p->dim > gScreenHeight-OFFSET) {
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
		p->spd += delta * 4;
		if (p->spd > p->dir.x)
			p->spd = p->dir.x;
	}
		
	else if (gLeft) {
		p->spd -= delta * 4;
		if (p->spd < -p->dir.x)
			p->spd = -p->dir.x;
	}
	else if (p->spd != 0) {
		p->spd += (p->spd < 0) ? 
			delta * 4 : delta * -4;
	}
	p->pos.x += p->spd*PLAT_SPD*delta;
	
	

	if (p->pos.x + p->w > gScreenWidth-OFFSET) {
		p->spd = 0;
		p->pos.x = gScreenWidth-OFFSET-p->w;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], 0); */ // nao seria uma boa ideia colocar o som de colisao da bolinha pra plataforma
	}

	if (p->pos.x < OFFSET) {
		p->spd = 0;
		p->pos.x = OFFSET;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], 0); */ //idem
	}
}

double dotProduct(VETOR2D a, VETOR2D b) {
	return (a.x * b.x)+(a.y * b.y);
}

double normalize (VETOR2D* a) {
	double norm = sqrt((a->x * a->x) + (a->y * a->y));
	a->x = a->x / norm;
	a->y = a->y / norm;
	return norm;
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
			/* move cada bola e 
			 * colide com o canto da tela
			 * e a plataforma
			 * e os blocos */
			collPlatBall(gBolas+i, delta);
			
			for (j = 0; j < gNumBlocos; j++) {
				if (gBlocos[j].vida) {
					collBallBlock(gBolas+i, gBlocos+j, delta);
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

BLOCO createBloco(VETOR2D pos, int tipo, int w, int h, SDL_Surface* img){
	BLOCO bloco;
	bloco.pos = pos;
	bloco.tipo = tipo;
	bloco.w = w;
	bloco.h = h;
	bloco.vida = 1;
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
	SDL_FreeSurface(gBallImgs[0]);
	SDL_FreeSurface(gPadImgs[0]);

	gBallImgs[0] = NULL;
	gPadImgs[0] = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	/*Mix_FreeChunk(gSons[0]);
	gSons[0] = NULL;*/

	Mix_CloseAudio();
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

int render() {
	SDL_Rect srcRect, dstRect;
	int i, err = false;

	//Fill the surface white
	SDL_FillRect( gScreenSurface, NULL,
	SDL_MapRGB( gScreenSurface->format,
				0, 0, 0 ) );

	srcRect.x = 24; srcRect.y = 24;
	srcRect.w = 592; srcRect.h = 432;
	SDL_FillRect( gScreenSurface, &srcRect,
	SDL_MapRGB( gScreenSurface->format,
				0xBB, 0xBB, 0xBB ) );

	srcRect.x = 32; srcRect.y = 32;
	srcRect.w = 576; srcRect.h = 416;
	SDL_FillRect( gScreenSurface, &srcRect,
	SDL_MapRGB( gScreenSurface->format,
				0, 0, 0 ) );

	srcRect.x = 0; srcRect.y = 0;

	/* Renderiza os blocos */
	for(i = 0; i < gNumBlocos; i++) {
		
		if (gBlocos[i].vida){
					
			srcRect.w = gBlocos[i].w;
			srcRect.h = gBlocos[i].h;

			dstRect.x = gBlocos[i].pos.x;
			dstRect.y = gBlocos[i].pos.y;

			if( SDL_BlitSurface( gBlocos[i].img, &srcRect,
								gScreenSurface, &dstRect ) < 0 ) {
				fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
				err = true;
			}
			
		}
		
	}

	/* Renderiza as bolas */
	for(i = 0; i < gNumBolas; i++) {
		srcRect.w = gBolas[i].dim;
		srcRect.h = gBolas[i].dim;

		dstRect.x = gBolas[i].pos.x;
		dstRect.y = gBolas[i].pos.y;
		
		if (gBolas[i].ativo){
			if( SDL_BlitSurface( gBolas[i].img, &srcRect,
								gScreenSurface, &dstRect ) < 0 ) {
				fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
				err = true;
			}
		}
	}

	/* Renderiza a plataforma */
	srcRect.w = gPad[0].w;
	srcRect.h = gPad[0].h;

	dstRect.x = gPad[0].pos.x;
	dstRect.y = gPad[0].pos.y;

	if( SDL_BlitSurface( gPad[0].img, &srcRect,
							gScreenSurface, &dstRect ) < 0 ) {
			fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
            err = true;
		}

    //Update the surface
    SDL_UpdateWindowSurface( gWindow );

	return err;
}

void collBallPlat(BOLA* a, double delta){
	double dx, dy;
	VETOR2D c;
	int inv = 0;
	c.x = a->pos.x + a->dim/2;
	c.y = a->pos.y + a->dim/2;
	if (!isInAABB(c, gPad->pos.x - a->dim/2, 
						  gPad->pos.y - a->dim/2, 
						  gPad->pos.x + gPad->w + a->dim/2, 
						  gPad->pos.y + gPad->h + a->dim/2)) {
		return;
	} 
	if (isInAABB(c, gPad->pos.x - a->dim/2, 
						 gPad->pos.y, 
						 gPad->pos.x + gPad->w + a->dim/2, 
						 gPad->pos.y + gPad->h)) {
		a->dir.x = -a->dir.x;
		a->pos.x += a->dir.x*a->spd*delta;
		
	}
	else if (isInAABB(c, gPad->pos.x, 
							   gPad->pos.y - a->dim/2, 
							   gPad->pos.x + gPad->w, 
							   gPad->pos.y + gPad->h + a->dim/2)) {
		a->dir.y = -a->dir.y;
		a->pos.y += a->dir.y*a->spd*delta;
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
		dy += gPad->h;
		if (a->dim*a->dim < (dx*dx)+(dy*dy)) {
			inv = 1;
		}
		dx -= gPad->w;
		if (a->dim*a->dim < (dx*dx)+(dy*dy)) {
			inv = 1;
		}
		if (inv) {
			a->dir.x = -a->dir.x;
			a->dir.y = -a->dir.y;
			a->pos.x += a->dir.x*a->spd*delta;
			a->pos.y += a->dir.y*a->spd*delta;
		}
		
	}
		/*a->dir.y = -a->dir.y;
		a->pos.y = gPad->pos.y-10;*/

	/*dix = (a->posx +(IMAGE_WIDTH /2) - gPad.w);
	diy = (a->posy +(IMAGE_HEIGHT/2) - gPad.h);
	dir = sqrt((dix * dix) + (diy * diy));
	if(dir <= ())*/
}

void collPlatBall(BOLA* a, double delta) {
	
	double dx, dy;
	VETOR2D c;
	int inv = 0;
	c.x = a->pos.x + a->dim/2;
	c.y = a->pos.y + a->dim/2;
	if (!isInAABB(c, gPad->pos.x - a->dim/2, 
						  gPad->pos.y - a->dim/2, 
						  gPad->pos.x + gPad->w + a->dim/2, 
						  gPad->pos.y + gPad->h + a->dim/2)) {
		return;
	} 
	if (isInAABB(c, gPad->pos.x - a->dim/2, 
						 gPad->pos.y, 
						 gPad->pos.x + gPad->w + a->dim/2, 
						 gPad->pos.y + gPad->h)) {
		if (gPad->dir.x > 0) {
			a->pos.x = gPad->pos.x + gPad->w + 1;
			a->dir.x = fabs(a->dir.x);
		}
		else {
			a->pos.x = gPad->pos.x - a->dim - 1;
			a->dir.x = -fabs(a->dir.x);
		}
		
	}
	else if (isInAABB(c, gPad->pos.x, 
							   gPad->pos.y - a->dim/2, 
							   gPad->pos.x + gPad->w, 
							   gPad->pos.y + gPad->h + a->dim/2)) {
		if (c.y > gPad->pos.y + gPad->h/2) {
			a->pos.y = gPad->pos.y + gPad->h + 1;
			a->dir.y = fabs(a->dir.y);
		}
		else {
			a->pos.y = gPad->pos.y - a->dim - 1;
			a->dir.y = -fabs(a->dir.y);
		}
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
		dy += gPad->h;
		if (a->dim*a->dim < (dx*dx)+(dy*dy)) {
			inv = 1;
		}
		dx -= gPad->w;
		if (a->dim*a->dim < (dx*dx)+(dy*dy)) {
			inv = 1;
		}
		if (inv) {
			if (gPad->dir.x > 0) {
				a->pos.x = gPad->pos.x + gPad->w + 1;
				a->dir.x = fabs(a->dir.x);
			}
			else {
				a->pos.x = gPad->pos.x - a->dim - 1;
				a->dir.x = -fabs(a->dir.x);
			}
			if (c.y > gPad->pos.y + gPad->h/2) {
				a->pos.y = gPad->pos.y + gPad->h + 1;
				a->dir.y = fabs(a->dir.y);
			}
			else {
				a->pos.y = gPad->pos.y - a->dim - 1;
				a->dir.y = -fabs(a->dir.y);
			}
		}
	}
	
	/*if ( a->pos.y + a->dim/2 >= gPad->pos.y &&
			a->pos.y + a->dim/2 >= gPad->pos.y + gPad->h &&
			a->pos.x + a->dim/2 >= gPad->pos.x && 
			a->pos.x + a->dim/2 <= gPad->pos.x + gPad->w) {
		
		if (gPad->dir.x > 0) {
			a->pos.x = gPad->pos.x + gPad->w + 1;
			a->dir.x = fabs(a->dir.x);
		}
		else {
			a->pos.x = gPad->pos.x - a->dim - 1;
			a->dir.x = -fabs(a->dir.x);
		}
		
	}*/
}

int loadBlocosFromFile(char* levelName) {
	FILE* arq;
	char* path;
	char* linha;
	int i;
	int lc; /*linha count*/
	
	path = malloc((strlen(levelName) + 17)*sizeof(char));
	linha = calloc(BLOCKS_W+2, sizeof(char));
	path[0] = '\0';
	
	strcat(path, "./data/level/");
	strcat(path, levelName);
	strcat(path, ".dat");
	if (!(arq = fopen(path, "r"))) {
		perror("Erro carregando bloco");
		return false;
	}
	lc = 0;
	while(fgets(linha, BLOCKS_W+2, arq) != NULL && lc <= BLOCKS_H) {
		VETOR2D pos;
		char c;
		for (i = 0; i <= BLOCKS_W; i++) {
			c = linha[i];
			if (c >= '0' && c <= '9') {
				pos.x = i*(BLOCK_DIST+32)+OFFSET+BLOCK_DIST;
				pos.y = lc*(BLOCK_DIST+16)+OFFSET+BLOCK_DIST;
				gBlocos[gNumBlocos++] = 
				createBloco(pos, c-'0', 32, 16, gBlocoImgs[c-'0']);
			}
		}
		lc++;
	}
	return true;
}

void collBallBlock(BOLA* a, BLOCO* b, double delta) {
	double dx, dy;
	VETOR2D c;
	int inv;
	c.x = a->pos.x + a->dim/2;
	c.y = a->pos.y + a->dim/2;
	if (!isInAABB(c, b->pos.x - a->dim/2, 
						  b->pos.y - a->dim/2, 
						  b->pos.x + b->w + a->dim/2, 
						  b->pos.y + b->h + a->dim/2)) {
		return;
	} 
	if (isInAABB(c, b->pos.x - a->dim/2, 
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
		if (!inv) return;
	}
	b->vida--;
}

int collBallPoint(BOLA* a, double dx, double dy, double delta) {
	if (a->dim*a->dim < (dx*dx)+(dy*dy)) {
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
		return true;
	}
	return false;
}

double sqDist(VETOR2D a, VETOR2D b) {
	double dx, dy;
	dx = a.x - b.x;
	dy = a.y - b.y;
	return (dx*dx)+(dy*dy);
}

int isInAABB(VETOR2D t, double p1x, double p1y, double p4x, double p4y) {
	return  t.x <= p4x && t.x >= p1x &&
			t.y <= p4y && t.y >= p1y;
}
