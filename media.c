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
#include <errno.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "defs.h"
#include "global.h"
#include "media.h"
#include "game.h"

int loadMedia() {
	uint32_t colorKey;
    int i;

	/* CARREGANDO FONTES */

	if (!(gScoreFonte = loadFont("./data/DS-DIGI.TTF", 36))) return false;

	/* FIM CARREGANDO FONTES */

	/* CARREGANDO IBAGENS */

		/*
		 * ColorKey eh magenta
		 */

    /* Carrega plataformas */
    if( !(gPadImgs[0] = loadSurface("./data/pad.png")) ) return false;
    colorKey = SDL_MapRGB(gPadImgs[0]->format, 0xFF, 0x00, 0xFF );
    SDL_SetColorKey(gPadImgs[0], SDL_TRUE, colorKey);

    /* Carrega bolas */
    if ( !(gBallImgs[0] = loadSurface("./data/ball.png")) ) return false;
    colorKey = SDL_MapRGB(gBallImgs[0]->format, 0xFF, 0x00, 0xFF );
    SDL_SetColorKey(gBallImgs[0], SDL_TRUE, colorKey);

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

    if ( !(gBlocoBreak = loadSurface("./data/brickBreakingTexture.png")) ) return false;

    /* Carrega Leds */
    if ( !(gLed[0] = loadSurface("./data/ledOn.png")) ) return false;
    if ( !(gLed[1] = loadSurface("./data/ledOff.png")) ) return false;

    /* Carrega PowerUps */
    if ( !(gPWPImgs[0] = loadSurface("./data/pwpDeath.png")) ) return false;
    if ( !(gPWPImgs[1] = loadSurface("./data/pwpPad.png")) ) return false;
    if ( !(gPWPImgs[2] = loadSurface("./data/pwpBallDup.png")) ) return false;
    if ( !(gPWPImgs[3] = loadSurface("./data/pwpBallSpdUp.png")) ) return false;

    for (i=0; i<10; i++){
		colorKey = SDL_MapRGB(gBlocoImgs[i]->format, 0xFF, 0x00, 0xFF );
		SDL_SetColorKey(gBlocoImgs[i], SDL_TRUE, colorKey);
	}

	colorKey = SDL_MapRGB(gBlocoBreak->format, 0xFF, 0x00, 0xFF );
	SDL_SetColorKey(gBlocoBreak, SDL_TRUE, colorKey);

    /* FIM CARREGANDO IBAGENS */

    /* CARREGANDO SONS */

    /* volume do som varia entre 0 e 127 */
    if ( !(gSons[SOUND_PLAT] = loadSound("./data/sound/plat_hit.wav")) ) return false;
    Mix_VolumeChunk(gSons[SOUND_PLAT], 64);
    
    if ( !(gSons[SOUND_WALL] = loadSound("./data/sound/bump.wav")) ) return false;
    Mix_VolumeChunk(gSons[SOUND_WALL], 96);
    
    if ( !(gSons[SOUND_BLOCK_BROKE] = loadSound("./data/sound/block_pop.wav")) ) return false;
    Mix_VolumeChunk(gSons[SOUND_BLOCK_BROKE], 96);
    
    if ( !(gSons[SOUND_LIFE_LOST] = loadSound("./data/sound/death.wav")) ) return false;
    Mix_VolumeChunk(gSons[SOUND_LIFE_LOST], 96);
    
    if ( !(gSons[SOUND_EXPLODE] = loadSound("./data/sound/explosion.wav")) ) return false;
    Mix_VolumeChunk(gSons[SOUND_EXPLODE], 128);

    /* FIM CARREGANDO SONS */
    return true;
}

Mix_Chunk* loadSound(char* path) {
	Mix_Chunk* sound = NULL;
	sound = Mix_LoadWAV(path);
	if (!sound) {
		fprintf(stderr, "Erro: incapaz de carregar som: %s\n%s\n",
						path, Mix_GetError() );

		gGameStatus = -301;
	}
	return sound;
}

TTF_Font* loadFont(char* path, int fontsize) {
	TTF_Font* fonte;
	fonte = TTF_OpenFont(path, fontsize);
	if (!fonte) {
		fprintf(stderr,"Impossivel abrir fonte! %s\n", TTF_GetError() );
		gGameStatus = -302;
	}
	return fonte;
}

SDL_Surface* loadSurface(char* path ) {
    /* A imagem otimizada final */
    SDL_Surface* optimizedSurface = NULL;

    /* Carrega imagem no PATH dado */
    SDL_Surface* loadedSurface = IMG_Load( path );
    if(!loadedSurface) {
        fprintf(stderr, "Erro: incapaz de carregar imagem: %s\n%s\n",
						path, IMG_GetError() );
		gGameStatus = -303;
    }
    else {
        /* Converte a imagem ao formato da tela */
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0 );
        if(!optimizedSurface) {
            fprintf(stderr, "Erro: incapaz de otimizar imagem: %s\n%s\n",
							path, SDL_GetError() );
			gGameStatus = -304;
        }

        /* Libera a imagem normal da memoria */
        SDL_FreeSurface( loadedSurface );
    }

    return optimizedSurface;
}

int loadBlocosFromNumber(int num) {
	char levelName[7];
	sprintf(levelName, "level%d", num);
	return loadBlocosFromFile(levelName);
}

int loadBlocosFromFile(char* levelName) {
	FILE* arq;
	char* path;
	char* linha;
	int i;
	int lc; /*linha count*/

	gNumBlocos = 0;

	path = malloc((strlen(levelName) + 17)*sizeof(char));
	linha = calloc(BLOCKS_W+2, sizeof(char));
	path[0] = '\0';

	strcat(path, "./data/level/");
	strcat(path, levelName);
	strcat(path, ".dat");

	if (!(arq = fopen(path, "r"))) {
		perror("Erro carregando bloco: ");

		gGameStatus = -abs(errno);
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
					createBloco(pos, c-'0', 32, 16, blocoLife(c-'0'), gBlocoImgs[c-'0']);

			}
		}
		lc++;
	}
	gNumBlocosAlive = gNumBlocos;
	fclose(arq);
	return true;
}

int blocoLife(int tipo){

	static int tipos[10] = {1,1,1,1,1,2,2,1,4,8};

	return tipos[tipo];

}

void readPlayers() {
	int i;
	for (i = 0; i < 5; i++) {
		gPlayers[i].name = calloc(12, sizeof(char)); /* 12 = MAXLEN */
		fread(gPlayers[i].name, sizeof(char), 12, gRank);
		fread(&gPlayers[i].pts, sizeof(int), 1, gRank);
		fread(&gPlayers[i].sysTime, sizeof(long), 1, gRank);
	}
}

void writePlayers() {
	int i;
	for (i = 0; i < 5; i++) {
		fwrite(gPlayers[i].name, sizeof(char), 12, gRank);
		fwrite(&gPlayers[i].pts, sizeof(int), 1, gRank);
		fwrite(&gPlayers[i].sysTime, sizeof(long), 1, gRank);
	}
}

int loadRanking() {
	/* poe o hiscore */
	gRank = fopen("./data/rank/rank.bin","rb");
	if(!gRank){
		puts("Impossivel abrir arquivo do rank!");
		gGameStatus = -666;
		return false;
	}

	readPlayers();
  
	fclose(gRank);
	return true;
}
