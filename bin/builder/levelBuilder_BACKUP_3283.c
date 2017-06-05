/*
 * levelBuilder.c
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
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

const int MAX_NUM_BLOCOS = 100;

/* Tipos de bloco/bola */

/* Structs */
typedef struct _VETOR2D {
	double x;
	double y;
} VETOR2D;

typedef struct _CURSOR{
	VETOR2D pos;
	int tipo;
} CURSOR;

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

SDL_Surface* gBlocoImgs[10];

int gNumBlocos = 0;

CURSOR* gCursor;
BLOCO* gBlocos;

/* Funcoes da main.c */
int createNPCs();

/* Inicia o jogo */
int init();

/* Carrega as imagens e audio */
int loadMedia();

/* Carrega uma imagem */
SDL_Surface* loadSurface (char* path);

/*Cria um bloco*/
BLOCO createBloco(VETOR2D pos, int tipo, int w, int h, SDL_Surface* img);


/* Renderiza a tela */
int render();

/* Processa eventos */
int handleEvent(SDL_Event* e);
int handleInput(SDL_Event* evt);

/* Carrega blocos de um level *
 * O level deve estar em ./data/level/ *
 * Retorna true se leu de boas, false se deu m */
char gArqAlvo[64];
int loadBlocosFromFile(char* levelName);
int saveBlocosOnFile(char* target);

/* Encerra os sistemas e sai do jogo */
void exitGame();

/* Fim das funcoes da main.c */

int main(int argc, char **argv) {
	
	char arqOrig[64];
	
	printf("Arquivo de origem:\n");
	scanf("%s", arqOrig);
	printf("Arquivo de destino:\n");
	scanf("%s", gArqAlvo);

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
<<<<<<< HEAD

	gCursor->pos.x = OFFSET+5;
	gCursor->pos.y = OFFSET+5;

	loadBlocosFromFile("level1");
=======
	
	gCursor->pos.x = OFFSET + BLOCK_DIST;
	gCursor->pos.y = OFFSET + BLOCK_DIST;
	
	loadBlocosFromFile(arqOrig);
>>>>>>> 30468455df0c2fbe03e6c39367c88c569b5e8a38

	quit = false;
	lastTime = currentTime = startTime = SDL_GetTicks();

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
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Erro: SDL falhou ao iniciar: %s\n",
			SDL_GetError());
		return false;
	}

	gWindow = SDL_CreateWindow("Breakout Work-in-Progress",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		gScreenWidth, gScreenHeight,
		SDL_WINDOW_SHOWN|SDL_WINDOW_INPUT_FOCUS);

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

double dotProduct(VETOR2D a, VETOR2D b) {
	return (a.x * b.x)+(a.y + b.y);
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


int createNPCs() {

	gCursor = calloc(1, sizeof(BLOCO));
	if (!gCursor) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		return false;
	}
<<<<<<< HEAD

	gBlocos = calloc(MAX_NUM_BLOCOS, sizeof(BLOCO));
=======
	
	gBlocos = calloc(BLOCKS_W * BLOCKS_H, sizeof(BLOCO));
>>>>>>> 30468455df0c2fbe03e6c39367c88c569b5e8a38
	if (!gBlocos) {
		fprintf(stderr, "Erro: Problema alocando memoria:\n%s\n", strerror(errno));
		return false;
	}

	return true;
}

void exitGame() {
	int i;
	for (i=0; i<5; i++){
		SDL_FreeSurface(gBlocoImgs[i]);
		gBlocoImgs[i]=NULL;
	}

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

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
				if (gCursor->pos.x - (32+BLOCK_DIST) > OFFSET)
					gCursor->pos.x -= (32+BLOCK_DIST);
			}
			if (e.key.keysym.sym == SDLK_RIGHT) {
				if (gCursor->pos.x + (32+BLOCK_DIST) < OFFSET + BLOCKS_W*(32+BLOCK_DIST))
					gCursor->pos.x += (32+BLOCK_DIST);
			}
			if (e.key.keysym.sym == SDLK_UP) {
				if (gCursor->pos.y - (16+BLOCK_DIST) > OFFSET)
					gCursor->pos.y -= (16+BLOCK_DIST);
			}
			if (e.key.keysym.sym == SDLK_DOWN) {
				if (gCursor->pos.y + (16+BLOCK_DIST) < OFFSET + BLOCKS_H*(16+BLOCK_DIST))
					gCursor->pos.y += (16+BLOCK_DIST);
			}
			if (e.key.keysym.sym == SDLK_z) {
				gCursor->tipo--;
				if (gCursor->tipo < 0)
					gCursor->tipo = 9;
			}
			if (e.key.keysym.sym == SDLK_x) {
				gCursor->tipo++;
				if (gCursor->tipo > 9)
					gCursor->tipo = 0;
			}
			if (e.key.keysym.sym == SDLK_c) {
				for(i = 0; i < gNumBlocos; i++){
					if(gBlocos[i].pos.x == gCursor->pos.x
					&& gBlocos[i].pos.y == gCursor->pos.y){
						gBlocos[i].ativo = true;
						gBlocos[i].tipo = gCursor->tipo;
						gBlocos[i].img = gBlocoImgs[gCursor->tipo];
					}
				}				
			}
			if (e.key.keysym.sym == SDLK_SPACE) {
				for(i = 0; i < gNumBlocos; i++){
					if(gBlocos[i].pos.x == gCursor->pos.x
					&& gBlocos[i].pos.y == gCursor->pos.y){
						gBlocos[i].ativo = false;
					}
				}	
			}
			if (e.key.keysym.sym == SDLK_s) {
				saveBlocosOnFile(gArqAlvo);
			}
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				quit = true;
			}
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
<<<<<<< HEAD

=======
				
	srcRect.w = gBlocos[0].w;
	srcRect.h = gBlocos[0].h;

	dstRect.x = OFFSET + BLOCK_DIST;
	dstRect.y = STD_SCREEN_HEIGHT - OFFSET - BLOCK_DIST - 16;
	
	if( SDL_BlitSurface( gBlocoImgs[gCursor->tipo], &srcRect,
						gScreenSurface, &dstRect ) < 0 ) {
		fprintf(stderr, "Erro: SDL nao blitou: %s\n", SDL_GetError() );
		err = true;
	}
	
>>>>>>> 30468455df0c2fbe03e6c39367c88c569b5e8a38
	srcRect.x = gCursor->pos.x;  srcRect.y = gCursor->pos.y+8;
	srcRect.w = 32;    srcRect.h = 8;

	SDL_FillRect( gScreenSurface, &srcRect,
	SDL_MapRGB( gScreenSurface->format,
				0xFF, 0xFF, 0xFF ) );
	
    //Update the surface
    SDL_UpdateWindowSurface( gWindow );

	return err;
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
<<<<<<< HEAD
			if (c >= '0' && c < '5') {
				pos.x = i*(BLOCK_DIST+32)+OFFSET+5;
				pos.y = lc*(BLOCK_DIST+16)+OFFSET+5;
				gBlocos[gNumBlocos++] =
					createBloco(pos, c-'0', 32, 16, gBlocoImgs[c-'0']);
=======
			if (c >= '0' && c <= '9') {
				pos.x = i*(BLOCK_DIST+32)+OFFSET+BLOCK_DIST;
				pos.y = lc*(BLOCK_DIST+16)+OFFSET+BLOCK_DIST;
				gBlocos[gNumBlocos++] = 
				createBloco(pos, c-'0', 32, 16, gBlocoImgs[c-'0']);
>>>>>>> 30468455df0c2fbe03e6c39367c88c569b5e8a38
			}
		}
		putchar('\n');
		lc++;
	}
	return true;
}

int saveBlocosOnFile(char* target) {
	FILE* arq;
	char* path;
	int i, lc; /*linha count*/
	int b;
	
	path = malloc((strlen(target) + 17)*sizeof(char));
	path[0] = '\0';
	
	strcat(path, "./data/level/");
	strcat(path, target);
	strcat(path, ".dat");
	if (!(arq = fopen(path, "w"))) {
		perror("Erro carregando bloco");
		return false;
	}
	
	for (lc = OFFSET + BLOCK_DIST; lc <= OFFSET + BLOCKS_H*(16+BLOCK_DIST); lc += 16 + BLOCK_DIST){
		for (i = OFFSET + BLOCK_DIST; i <= OFFSET + BLOCKS_W*(32+BLOCK_DIST); i += 32 + BLOCK_DIST){
			for (b = 0; b < gNumBlocos; b++){
				if(gBlocos[b].pos.x == i
				&& gBlocos[b].pos.y == lc){
					if (gBlocos[b].ativo)
						fprintf(arq, "%c", gBlocos[b].tipo+'0');
					else
						fprintf(arq, "-");
				}
			}
		}
		fprintf(arq, "\n");
	}
	
	return true;
}
