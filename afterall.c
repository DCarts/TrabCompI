/*
 *afterall.c
 *Neste arquivo se encontram as funções que deverão ser executadas após o loop principal do jogo.
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */


 #include <stdio.h>
 #include <stdlib.h>
 #include <math.h>
 #include <errno.h>
 #include <string.h>
 #include <time.h>

 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
 #include <SDL2/SDL_mixer.h>
 #include <SDL2/SDL_ttf.h>

 #include "defs.h"
 #include "global.h"
 #include "render.h"
 #include "afterall.h"

 #define MAXLEN 12

static char gameOverMSG[22] = "Digite seu nome:";
static char standardMessage[22] = "Digite seu nome:";
static char namae[MAXLEN] = "ze sa";
static char blank[] = "  ";
static SDL_Surface* standardMSurface = NULL;
static SDL_Surface* clipboardSurface = NULL;
static SDL_Event ev;
static SDL_Rect dstRect;

int setClipboard(int gameOver) {	/* função para capturar a entrada do nome do player e blitar diamicamente na tela */
	long countTime, currentTime;
	int ableRender;
	int leave = false;
	int err = false, cont = strlen(namae);
	SDL_Surface* tmpScoreSurface = NULL;
	char tmpScoreText[25];
	char *txtToRender;

	sprintf(tmpScoreText, "%s %s", "Seu score:", gScoreText);

	currentTime = countTime = SDL_GetTicks();
	countTime--; /*pra renderizar no comeco*/

	/*	renderizando a mensagem padrão na superfície	*/
	if(!(standardMSurface = TTF_RenderText_Shaded(gScoreFonte,standardMessage,gScoreFontColor,gBgColor))) {
		fprintf(stderr,"Impossivel renderizar texto standardMessage na tela!%s\n",TTF_GetError());
		gGameStatus = -101;
		err = true;
	}

	/*	renderizando a mensagem padrão de score na superfície	*/
	if(!(tmpScoreSurface = TTF_RenderText_Shaded(gScoreFonte,tmpScoreText,gScoreFontColor,gBgColor))) {
		fprintf(stderr,"Impossivel renderizar texto standardMessage na tela!%s\n",TTF_GetError());
		gGameStatus = -102;
		err = true;
	}

	SDL_StartTextInput();	/* habilitando a entrada de texto pelo usuário	*/

	while(!leave) {
		ableRender = false;
		currentTime = SDL_GetTicks();

		if (countTime < currentTime) {
			flip = 1 - flip;
			ableRender = true;
			countTime = currentTime + 1000;
		}

		while(SDL_PollEvent(&ev) != 0) {
			if(ev.type == SDL_QUIT) {
				gGameStatus = 300;
				SDL_StopTextInput();
				return 1;
			}
			if(ev.type == SDL_KEYDOWN) {
				if (ev.key.keysym.sym == SDLK_ESCAPE) {
					gGameStatus = 300;
					SDL_StopTextInput();
					return 1;
				}
				if(ev.key.keysym.sym == SDLK_BACKSPACE && cont > 0) {
					namae[--cont] = '\0';
					ableRender = true;
				}
				else if(ev.key.keysym.sym == SDLK_RETURN || ev.key.keysym.sym == SDLK_RETURN2) {
					puts("p");
					leave = true;
				}
			}
			else if(ev.type == SDL_TEXTINPUT) {
				if(!( ( ev.text.text[ 0 ] == 'c' || ev.text.text[ 0 ] == 'C' )
						&& ( ev.text.text[ 0 ] == 'v' || ev.text.text[ 0 ] == 'V' )
						&& SDL_GetModState() & KMOD_CTRL )
						&& cont < MAXLEN-1) {
					namae[cont++] = ev.text.text[0];
					namae[cont] = '\0';
					ableRender = true;
				}
			}

			if(strlen(namae) > 0) {
				ableRender = true;
			}

			/* literalmente a pegunta "posso renderizar o texto?"*/
			if (ableRender) {

				if (strlen(namae) > 0) {
					txtToRender = namae;
				}
				else {
					txtToRender = blank;
				}

				/*	Renderizando o texto de entrada na superfície	*/
				if (!(clipboardSurface = TTF_RenderText_Shaded(gScoreFonte,txtToRender,gScoreFontColor,gBgColor))) {
					fprintf(stderr,"Impossivel renderizar texto de entrada na tela!%s\n",TTF_GetError());
					gGameStatus = -103;
					err = true;
					break;
				}

				/* agora realmente renderizando (blitando) */

				/* Tornando a superfície escura novamente */
				SDL_FillRect( gScreenSurface, NULL,
						SDL_MapRGB( gScreenSurface->format, 0, 0, 0 ) );

				/*else{	printf("Renderizei ok\n");}*/

				dstRect.x = gScreenWidth/2 - standardMSurface->w/2;
				dstRect.y = 36;	/*	definindo a posição do retangulo do txt padrao */

				/*	blitando a msg padrao	*/
				if (SDL_BlitSurface(standardMSurface, NULL, gScreenSurface,&dstRect) < 0) {
					fprintf(stderr,"Impossivel blitar texto padrão na tela! %s\n",SDL_GetError());
					gGameStatus = -104;
					err = true;
				}

				dstRect.x = gScreenWidth/2 - tmpScoreSurface->w/2;
				dstRect.y = 0;	/*	definindo a posição do retangulo do score */

				/*	blitando o score */
				if (SDL_BlitSurface(tmpScoreSurface, NULL, gScreenSurface,&dstRect) < 0) {
					fprintf(stderr,"Impossivel blitar texto padrão na tela! %s\n",SDL_GetError());
					gGameStatus = -105;
					err = true;
				}

				dstRect.x = gScreenWidth/2 - clipboardSurface->w/2;
				dstRect.y = gScreenHeight/2 - clipboardSurface->h/2; /*	definindo a posição do texto sendo escrito */

				/*	blitando o texto sendo escrito	*/
				if (SDL_BlitSurface(clipboardSurface, NULL, gScreenSurface, &dstRect) < 0) {
					fprintf(stderr,"Impossivel blitar texto de entrada na tela! %s\n",SDL_GetError());
					gGameStatus = -106;
					err = true;
					break;
				}
				/*else{	printf("Blitei ok\n");}*/

				SDL_UpdateWindowSurface(gWindow);
				/*printf("%s\n",namae);*/
			}
		}
	}

	SDL_StopTextInput(); /*	encerrando a entrada de texto	*/

	if(err){
		printf("Erro ao renderizar clipboard\n");
		return 0;
	}

	savePlayer(namae);

	return 1;
}


void savePlayer(char* namae) {

	gGameStatus += 1;
	SCOREENTRY current;

	current.name = namae;
	current.pts = gPlayer.pontos;
	current.sysTime = time(NULL);

	gRank = fopen("./data/rank/rank.bin","a");
	if(!gRank){
		puts("Impossivel abrir arquivo do rank!");
		exit(666);
	}
	
	//fread(gPlayers, sizeof(SCOREENTRY), 5, gRank);
	//players[5] = current;
	
	//qsort(gPlayers, 6, sizeof(SCOREENTRY), sortByScore);

	/* ta assim pra criar as entradas padrao no rank.bin
	 * qnd tiver pronto, tem que alterar pra usar o gPlayers e
	 * alterar no init() pra carregar o gPlayers */
	fwrite(&current, 1, sizeof(SCOREENTRY), gRank);	/*	grava o nome do jogador no arquivo apontado por gRank	*/

	gPlayer.vidas = 3;
	gPlayer.pontos = 0;
	gGameStatus += 1;
}

int sortByScore(SCOREENTRY* a, SCOREENTRY* b) {
	if (a->pts == b->pts) return a->sysTime < b->sysTime;
	else return a->pts > b->pts;
}
