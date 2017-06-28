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
 #include "game.h"
 #include "media.h"

 #define MAXLEN 12

static char gameOverMSG[22] = "Digite seu nome:";
static char standardMessage[22] = "Digite seu nome:";
static char namae[MAXLEN] = "ze sa";
static char blank[] = "  ";
static SDL_Surface* standardMSurface = NULL;
static SDL_Surface* clipboardSurface = NULL;
static SDL_Surface* tryAgSurface1 = NULL;
static SDL_Surface* tryAgSurface2 = NULL;
static SDL_Event ev;
static SDL_Rect dstRect;

int setClipboard(int gameOver) {	/* função para capturar a entrada do nome do player e blitar diamicamente na tela */
	long countTime, currentTime;
	int ableRender, sizeText;
	int leave = false;
	int err = false;
	int cont = strlen(namae);
	SDL_Surface* tmpScoreSurface = NULL;
	char tmpScoreText[25];
	char *txtToRender;
	char finalTxtToRender[MAXLEN+1];

	sprintf(tmpScoreText, "%s %s", "Seu score:", gScoreText);

	currentTime = countTime = SDL_GetTicks();
	countTime--; /*pra renderizar no comeco*/

	/*	renderizando a mensagem padrão na superfície	*/
    if (standardMSurface) SDL_FreeSurface(standardMSurface);
	if(!(standardMSurface = TTF_RenderText_Shaded(gScoreFonte,standardMessage,gScoreFontColor,gBgColor))) {
		fprintf(stderr,"Impossivel renderizar texto standardMessage na tela!%s\n",TTF_GetError());
		gGameStatus = -101;
		err = true;
	}

	/*	renderizando a mensagem padrão de score na superfície	*/
    if (tmpScoreSurface) SDL_FreeSurface(tmpScoreSurface);
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
				//return 1;
                exit(0);
			}
			if(ev.type == SDL_KEYDOWN) {
				/*if (ev.key.keysym.sym == SDLK_ESCAPE) {
					gGameStatus = 300;
					SDL_StopTextInput();
					return 1;
				}*/
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
		}

		/*if(strlen(namae) > 0) {
			ableRender = true;
		}*/

		/* literalmente a pegunta "posso renderizar o texto?"*/
		if (ableRender) {

			if (strlen(namae) > 0) {
				txtToRender = namae;
			}
			else {
				txtToRender = blank;
			}

			strcpy(finalTxtToRender, txtToRender);
			if (flip) {
				strcat(finalTxtToRender, "_");
			}

			/*	Renderizando o texto de entrada na superfície	*/
            if (clipboardSurface) SDL_FreeSurface(clipboardSurface);
			if (!(clipboardSurface = TTF_RenderText_Shaded(gScoreFonte,finalTxtToRender,gScoreFontColor,gBgColor))) {
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

			TTF_SizeText(gScoreFonte, txtToRender, &sizeText, NULL);

			dstRect.x = gScreenWidth/2 - sizeText/2;
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

	SDL_StopTextInput(); /*	encerrando a entrada de texto	*/

	if(err){
		printf("Erro ao renderizar clipboard\n");
		return 0;
	}

	savePlayer(namae);

	return 1;
}


void savePlayer(char* pName) {

	gGameStatus += 1;
	SCOREENTRY current;

	current.name = pName;
	current.pts = gPlayer.pontos;
	current.sysTime = time(NULL);

	gRank = fopen("./data/rank/rank.bin","wb");
	if(!gRank){
		puts("Impossivel abrir arquivo do rank!");
		exit(666);
	}

	gPlayers[5] = current;

	qsort(gPlayers, 6, sizeof(SCOREENTRY), sortByScore);

	/* ta assim pra criar as entradas padrao no rank.bin
	 * qnd tiver pronto, tem que alterar pra usar o gPlayers e
	 * alterar no init() pra carregar o gPlayers */

	//fwrite(gPlayers, 5, sizeof(SCOREENTRY), gRank);
	writePlayers();
	//fwrite(&current, 1, sizeof(SCOREENTRY), gRank);	/*	grava o nome do jogador no arquivo apontado por gRank	*/

	gPlayer.vidas = 3;
	gPlayer.pontos = 0;
	gGameStatus += 1;

	fclose(gRank);
}

int sortByScore(const void* aa, const void* bb) {
  SCOREENTRY *a, *b;
  a = (SCOREENTRY*)aa;
  b = (SCOREENTRY*)bb;
	if (a->pts == b->pts) return a->sysTime > b->sysTime;
	else return a->pts < b->pts;
}


/*  retorna != 0 se der ruim,retorna 0 se for de boas */
int tryAgain()
{
  SDL_Event event;
  SDL_Rect dstRect;
  char tryAgMessage [] = "Aperte ENTER para retornar ao menu.";
  char qMessage[] = "Aperte Q para sair do jogo.";
//  char defaultMessage[] = "Por favor,digite uma opção válida";
  int leave = false;

  /* Tornando a superfície escura novamente */
  SDL_FillRect( gScreenSurface, NULL,
      SDL_MapRGB( gScreenSurface->format, 0, 0, 0 ) );

  /*	Renderizando o texto de entrada na superfície	*/
  if (!(tryAgSurface1 = TTF_RenderText_Shaded(gScoreFonte,tryAgMessage,gScoreFontColor,gBgColor))) {
    fprintf(stderr,"Impossivel renderizar texto de tryagain na tela!%s\n",TTF_GetError());
    gGameStatus = -667;
    return 666;
  }

  /*	Renderizando o texto de entrada na superfície	*/
  if (!(tryAgSurface2 = TTF_RenderText_Shaded(gScoreFonte,qMessage,gScoreFontColor,gBgColor))) {
    fprintf(stderr,"Impossivel renderizar texto de quitgame na tela!%s\n",TTF_GetError());
    gGameStatus = -668;
    return 666;
  }

  dstRect.x = gScreenWidth/2 - tryAgSurface1->w/2;
  dstRect.y = gScreenHeight / 2;

  if(SDL_BlitSurface(tryAgSurface1,NULL,gScreenSurface,&dstRect) < 0)
  {
    fprintf(stderr,"Impossivel blitar texto de tryagain na tela!%s\n",SDL_GetError());
    gGameStatus = -669;
    return 666;
  }

  dstRect.x = gScreenWidth/2 - tryAgSurface2->w/2;
  dstRect.y = dstRect.y + 36;

  if(SDL_BlitSurface(tryAgSurface2,NULL,gScreenSurface,&dstRect) < 0)
  {
    fprintf(stderr,"Impossivel blitar texto de quitgame na tela!%s\n",SDL_GetError());
    gGameStatus = -670;
    return 666;
  }

  SDL_UpdateWindowSurface(gWindow);
  /*SDL_Delay(5000);*/

  while (!leave)
  {
    while(SDL_PollEvent(&event) != 0)
    {
      switch (event.type) {
        case SDL_KEYDOWN:
          if(event.key.keysym.sym == SDLK_q)
          {
            freeTryAgain();
            leave = true;
            return 8;
          }
          if(event.key.keysym.sym == SDLK_RETURN)
          {
            gGameStatus++;
            gTimesPlayed++;
            leave = true;
            freeTryAgain();
            return 0;
          }
          break;
        case SDL_QUIT: 
          freeTryAgain();
          leave = true;
          return 8;
      }
    }
  }
  return 8;
}


void freeTryAgain()
{
  SDL_FreeSurface(tryAgSurface1);
  SDL_FreeSurface(tryAgSurface2);
}
