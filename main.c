/*
 * main.c
 * 
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
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

const int STD_SCREEN_WIDTH = 640;
const int STD_SCREEN_HEIGHT = 480;

const int MAX_NUM_BLOCOS = 100;
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
	VETOR2D step;
	int tipo;
	int ativo;
	int dim;
	SDL_Surface* img;
} BOLA;

typedef struct _PLATAFORMA {
	VETOR2D pos;
	VETOR2D step;
	int melhorias[5];
	int ativo;
	int w;
	int h;
	SDL_Surface* img;
} PLATAFORMA;

typedef struct _BLOCO {
	VETOR2D pos;
	int tipo;
	int w, h;
	int ativo;
	SDL_Surface* img;
} BLOCO;

/* Variaveis Globais */

int gScreenWidth = 640;
int gScreenHeight = 480;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gBlocoImgs[5];
SDL_Surface* gBallImgs[5];
SDL_Surface* gPadImgs[5];

Mix_Chunk* gSons[10];

int gNumBolas = 6;
int gNumBlocos = 0;

PLATAFORMA* gPlataforma;
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
BOLA createBola(VETOR2D pos, VETOR2D step, int tipo, int dim, SDL_Surface* img);
/*Cria um bloco*/
BLOCO createBloco(VETOR2D pos, int tipo, int w, int h, SDL_Surface* img);
/*Cria a plataforma*/
PLATAFORMA createPlataforma(VETOR2D pos, VETOR2D step, int w, int h, SDL_Surface* img);

/* Move uma bolinha */
void move(BOLA* b);
/* Move a plataforma */
void movePlataforma(PLATAFORMA *p);

/* Loop do jogo */
int gameLoop();

/* Renderiza a tela */
int render();

/* Processa eventos */
int handleEvent(SDL_Event* e);
void handleInput(SDL_Event* evt);

/* Encerra os sistemas e sai do jogo */
void exitGame();

/* Funcoes de math uteis */
double normalize (VETOR2D* a);
double dotProduct(VETOR2D a, VETOR2D b);
void collide (BOLA* a, BOLA* b);

/* Fim das funcoes da main.c */

int main(int argc, char **argv) {
	
	int quit, startTime, currentTime;
	SDL_Event evt;
	
	if (!init() || !loadMedia()) {
		return 1;
	}
	
	createNPCs();
	
	quit = false;
	startTime = SDL_GetTicks();
	
	gLeft = false;
	gRight = false;
	
	/* Timer do jogo */
	while (!quit) {
		while (SDL_PollEvent(&evt) != 0) {
			quit = handleEvent(&evt);
			handleInput(&evt);
		}
		currentTime = SDL_GetTicks();
		if (startTime + 10 < currentTime) {
			startTime = currentTime;
			if (!gameLoop()) return 1;
		}
		if (render()) {puts("aaa"); return 1;}
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
    
    for (i=0; i<5; i++){
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

void move (BOLA* p) {
	p->pos.x = p->pos.x + p->step.x;
	p->pos.y = p->pos.y + p->step.y;
	
	if (p->pos.x + p->dim > gScreenWidth || p->pos.x < 0) {
		p->step.x = -p->step.x;
		p->pos.x += p->step.x;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], -1); */
	}
	if (p->pos.y + p->dim > gScreenHeight) {
		p->step.y = -p->step.y;
		p->pos.y += p->step.y;
		/* Mix_PlayChannel(-1, gSons[SOUND_FLOOR], -1); */
	}
	else if (p->pos.y < 0) {
		p->step.y = -p->step.y;
		p->pos.y += p->step.y;
		/* Mix_PlayChannel(-1, gSons[SOUND_TETO], -1); */
	}
}

void movePlataforma (PLATAFORMA* p) {
	
	if (gRight)
		p->pos.x += p->step.x;
	if (gLeft)
		p->pos.x -= p->step.x;
	
	if (p->pos.x + p->w > gScreenWidth) {
		p->pos.x -= p->step.x;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], -1); */
	}
	
	if (p->pos.x < 0) {
		p->pos.x += p->step.x;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], -1); */
	}
}

double dotProduct(VETOR2D a, VETOR2D b) {
	return (a.x * b.x)+(a.y + b.y);
}

double normalize (VETOR2D* a) {
	double norm = sqrt((a->x * a->x) + (a->y * a->y));
	a->x = a->x / norm;
	a->y = a->y / norm;
	return norm;
}

/* @todo fazer funcionar direito */
void collide(BOLA* a, BOLA* b) {
	VETOR2D c;
	double p;
	
	c.x = a->pos.x - b->pos.x;
	c.y = a->pos.y - b->pos.y;
	
	normalize(&c);
	
	p = (a->step.x * c.x) + (a->step.y * c.y) 
		- (b->step.x * c.x) - (b->step.y * c.y);
	
	a->step.x = a->step.x - p * c.x;
	a->step.y = a->step.y - p * c.y;
	b->step.x = a->step.x + p * c.x;
	b->step.y = a->step.y + p * c.y;
}

int gameLoop() {
	int i/*, j*/;
	/*double dx, dy, raios;*/
	
	for (i = 0; i < gNumBolas; i++) {
		move(gBolas+i);
	}
	/*    ainda WIP (colisao entre bolas)
	for (i = 0; i < gNumBolas; i++) {
		for (j = i+1; j < gNumBolas; j++) {
			
			dx = gBolas[i].pos.x-gBolas[j].pos.x;
			dy = gBolas[i].pos.y-gBolas[j].pos.y;
			raios = (gBolas[i].dim/2) + (gBolas[j].dim/2);
			
			if (raios*raios > dx*dx + dy*dy) {
				collide(gBolas+i, gBolas+j);
			}
		}
	}*/
	
	movePlataforma(gPlataforma);
	
	return true;
}

BOLA createBola(VETOR2D pos, VETOR2D step, int tipo, int dim, SDL_Surface* img) {
	BOLA bola;
	bola.pos = pos;
	bola.step = step;
	bola.tipo = tipo;
	bola.dim = dim;
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
	bloco.ativo = 1;
	bloco.img = img;
	return bloco;
}

PLATAFORMA createPlataforma(VETOR2D pos, VETOR2D step, int w, int h, SDL_Surface* img){
	PLATAFORMA plat;
	plat.pos = pos;
	plat.step = step;
	plat.w = 128;
	plat.h = 16;
	plat.img = img;
	plat.ativo = true;
	return plat;
}

int createNPCs() {
	VETOR2D pos, step;
	int i, j;
	int n;
	
	gBolas = calloc(MAX_NUM_BOLAS, sizeof(BOLA));
	if (!gBolas) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		return false;
	}
	gBlocos = calloc(MAX_NUM_BLOCOS, sizeof(BLOCO));
	if (!gBlocos) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		return false;
	}
	gPlataforma = calloc(1, sizeof(PLATAFORMA));
	if (!gPlataforma) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		return false;
	}
	
	/*
	 * Cria os blocos.
	 * Eh codigo temporario, so
	 * estou usando para testes
	*/
	n = 0 ;
	for (j=0; j<5; j++){
		for (i=0; i<20; i++){
			pos.x = i*32;
			pos.y = j*16;
			gBlocos[n] = createBloco(pos, 1, 32, 16, gBlocoImgs[j]);
			n++;
		}
	}

	for (i = 0; i < gNumBolas; i++) {
		pos.x = (rand() % gScreenWidth-10);
		pos.y = (rand() % gScreenHeight-10);
		step.x = (rand() % 2? -1 : 1);
		step.y = (rand() % 2? -1 : 1);
		gBolas[i] = createBola(pos, step, 1, 10, gBallImgs[0]);
	}
	
	pos.x = STD_SCREEN_WIDTH/2;
	pos.y = STD_SCREEN_HEIGHT-32;
	step.x = 4;
	step.y = 4;
	
	gPlataforma[0] = createPlataforma(pos, step, 64, 16, gPadImgs[0]);
	
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

void handleInput(SDL_Event* evt){
	SDL_Event e = *evt;
	
	switch (e.type) {
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_LEFT) {
				gLeft = true;
			}
			if (e.key.keysym.sym == SDLK_RIGHT) {
				gRight = true;
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
}

int handleEvent(SDL_Event* evt) {
	int quit = false;
	SDL_Event e = *evt;
	
	switch (e.type) {
		case SDL_QUIT:
			quit = true;
		break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				quit = true;
			}
		break;
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
	
	/* Renderiza as bolas */
	srcRect.x = 0; srcRect.y = 0;
	for(i = 0; i < gNumBolas; i++) {
		srcRect.w = gBolas[i].dim;
		srcRect.h = gBolas[i].dim;
		
		dstRect.x = gBolas[i].pos.x;
		dstRect.y = gBolas[i].pos.y;
		
		if( SDL_BlitSurface( gBolas[i].img, &srcRect, 
							gScreenSurface, &dstRect ) < 0 ) {
			fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
            err = true;
		}
	}
	
	/* Renderiza a plataforma */
	srcRect.w = gPlataforma[0].w;
	srcRect.h = gPlataforma[0].h;
		
	dstRect.x = gPlataforma[0].pos.x;
	dstRect.y = gPlataforma[0].pos.y;
	
	if( SDL_BlitSurface( gPlataforma[0].img, &srcRect, 
							gScreenSurface, &dstRect ) < 0 ) {
			fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
            err = true;
		}
		
	/* Renderiza os blocos */
	for(i = 0; i < MAX_NUM_BLOCOS; i++) {
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
            
    //Update the surface
    SDL_UpdateWindowSurface( gWindow );
	
	return err;
}
