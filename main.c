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

const int BLOCK_DIST = 6;
const int BLOCKS_W = 15;
const int BLOCKS_H = 5;

const int STD_SCREEN_WIDTH = 640;
const int STD_SCREEN_HEIGHT = 480;
const int OFFSET = 32;

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
	double spd;
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
PLATAFORMA createPlataforma(VETOR2D pos, VETOR2D step, SDL_Surface* img);

/* Move uma bolinha */
void moveBall(BOLA* b);
/* Move a plataforma */
void movePlataforma(PLATAFORMA *p);

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
void collide (BOLA* a, BOLA* b);
void detCollisionPlat(BOLA* a);

/* Fim das funcoes da main.c */

int main(int argc, char **argv) {

	int quit, startTime, currentTime, lastTime;
	SDL_Event evt;

	if (!init()) {
		return 1;
	}
	
	atexit(exitGame);
	
	if (!loadMedia()) {
		return 1;
	}

	createNPCs();
	loadBlocosFromFile("level1");

	quit = false;
	lastTime = currentTime = startTime = SDL_GetTicks();

	gLeft = false;
	gRight = false;

	/* Timer do jogo */
	while (!quit) {
		while (SDL_PollEvent(&evt) != 0) {
			quit = handleEvent(&evt);
		}
		lastTime = currentTime;
		lastTime++;
		currentTime = SDL_GetTicks();
		if (startTime + 10 < currentTime) {
			startTime = currentTime;
			if (gameLoop()) return 1;
		}
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

void moveBall(BOLA* p) {

	p->pos.x = p->pos.x + p->step.x;
	p->pos.y = p->pos.y + p->step.y;
	
	detCollisionPlat(p);

	if (p->pos.x + p->dim > gScreenWidth-OFFSET || p->pos.x < OFFSET) {
		p->step.x = -p->step.x;
		p->pos.x += p->step.x;
		/* Mix_PlayChannel(-1, gSons[SOUND_WALL], 0); */
	}
	if (p->pos.y + p->dim > gScreenHeight-OFFSET) {
		p->step.y = -p->step.y;
		p->pos.y += p->step.y;
		/* Mix_PlayChannel(-1, gSons[SOUND_FLOOR], 0); */
	}
	else if (p->pos.y < 32) {
		p->step.y = -p->step.y;
		p->pos.y += p->step.y;
		/* Mix_PlayChannel(-1, gSons[SOUND_TETO], 0); */
	}
}

void movePlataforma (PLATAFORMA* p) {
	
	if (gRight){
		p->spd += 0.5;
		if (p->spd > p->step.x)
			p->spd = p->step.x;
	}
		
	if (gLeft){
		p->spd -= 0.5;
		if (p->spd < -p->step.x)
			p->spd = -p->step.x;
	}
	
	p->pos.x += p->spd;
	
	if (p->spd > 0) p->spd -= 0.125;
	if (p->spd < 0) p->spd += 0.125;

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
		if (gBolas[i].ativo){
			moveBall(gBolas+i);
		}
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

	return false;
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

PLATAFORMA createPlataforma(VETOR2D pos, VETOR2D step, SDL_Surface* img){
	PLATAFORMA plat;
	plat.pos = pos;
	plat.step = step;
	plat.w = 96;
	plat.h = 12;
	plat.img = img;
	plat.spd = 0;
	plat.ativo = true;
	return plat;
}

int createNPCs() {
	VETOR2D pos, step;
	int i;

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

	for (i = 0; i < gNumBolas; i++) {
		pos.x = (rand() % (gScreenWidth-64))+32;
		pos.y = (rand() % (gScreenHeight-64))+32;
		step.x = (rand() % 2? -1 : 1);
		step.y = (rand() % 2? -1 : 1);
		gBolas[i] = createBola(pos, step, 1, 10, gBallImgs[0]);
	}

	pos.x = gScreenWidth/2;
	pos.y = gScreenHeight-56;
	step.x = 4;
	step.y = 4;

	gPlataforma[0] = createPlataforma(pos, step, gPadImgs[0]);

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
		
		if (gBlocos[i].ativo){
					
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
	srcRect.w = gPlataforma[0].w;
	srcRect.h = gPlataforma[0].h;

	dstRect.x = gPlataforma[0].pos.x;
	dstRect.y = gPlataforma[0].pos.y;

	if( SDL_BlitSurface( gPlataforma[0].img, &srcRect,
							gScreenSurface, &dstRect ) < 0 ) {
			fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
            err = true;
		}

    //Update the surface
    SDL_UpdateWindowSurface( gWindow );

	return err;
}

void detCollisionPlat(BOLA* a){
	if((a->pos.y + a->dim ) >= gPlataforma->pos.y){
		if(a->pos.x + a->dim >= gPlataforma->pos.x
		 && a->pos.x + a->dim <= gPlataforma->pos.x
		 + gPlataforma->w){
			 a->step.y = -a->step.y;
				a->pos.y = gPlataforma->pos.y-10;
		 }
	}

	/*dix = (a->posx +(IMAGE_WIDTH /2) - gPlataforma.w);
	diy = (a->posy +(IMAGE_HEIGHT/2) - gPlataforma.h);
	dir = sqrt((dix * dix) + (diy * diy));
	if(dir <= ())*/
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
		for (i = 0; i < BLOCKS_W; i++) {
			c = linha[i];
			if (c >= '0' && c < '5') {
				pos.x = i*(BLOCK_DIST+32)+OFFSET+5;
				pos.y = lc*(BLOCK_DIST+16)+OFFSET+5;
				gBlocos[gNumBlocos++] = 
					createBloco(pos, c-'0', 32, 16, gBlocoImgs[c-'0']);
			}
		}
		putchar('\n');
		lc++;
	}
	return true;
}
