/*
 * mainmenu.c
 * Aqui estão as funçoes do menu principal do jogo
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */


 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <time.h>
 #include <string.h>
 #include <errno.h>

 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
 #include <SDL2/SDL_mixer.h>
 #include <SDL2/SDL_ttf.h>

 #include "defs.h"
 #include "global.h"
 #include "render.h"
 #include "afterall.h"
 #include "game.h"
 #include "mainmenu.h"

static char gameName[] = "Breakout";
static char opOne[] = "1 -> Iniciar jogo";
static char opTwo[] = "2 -> Ranking";
static char opThree[] = "3 -> Sair do jogo";
static SDL_Surface* gameNameSurface = NULL;
static SDL_Surface* oneSurface = NULL;
static SDL_Surface* twoSurface = NULL;
static SDL_Surface* threeSurface = NULL;
static SDL_Surface* bestPlayersSurface[6];
static char txtToRender[5];

int menu()
{
  SDL_Event event;
  SDL_Rect dstRect;
  int leave = false;

  /* Tornando a superfície escura novamente */
  SDL_FillRect( gScreenSurface, NULL,
      SDL_MapRGB( gScreenSurface->format, 0, 0, 0 ) );

  /*	Renderizando o nome do jogo na superfície	*/
  if (!(gameNameSurface = TTF_RenderText_Shaded(gScoreFonte,gameName,gScoreFontColor,gBgColor))) {
    fprintf(stderr,"Impossivel renderizar nome do jogo na tela!%s\n",TTF_GetError());
    //gGameStatus = -667;
    return 666;
  }

  /*	Renderizando a opção 1 na superfície	*/
  if (!(oneSurface = TTF_RenderText_Shaded(gScoreFonte,opOne,gScoreFontColor,gBgColor))) {
    fprintf(stderr,"Impossivel renderizar superficie da opção 1 na tela!%s\n",TTF_GetError());
    //gGameStatus = -668;
    return 666;
  }

  /*	Renderizando a opção 2 na superfície	*/
  if (!(twoSurface = TTF_RenderText_Shaded(gScoreFonte,opTwo,gScoreFontColor,gBgColor))) {
    fprintf(stderr,"Impossivel renderizar superficie da opção 2 na tela!%s\n",TTF_GetError());
    //gGameStatus = -668;
    return 666;
  }

  /*	Renderizando a opção 3 na superfície	*/
  if (!(threeSurface = TTF_RenderText_Shaded(gScoreFonte,opThree,gScoreFontColor,gBgColor))) {
    fprintf(stderr,"Impossivel renderizar superficie da opção 3 na tela!%s\n",TTF_GetError());
    //gGameStatus = -668;
    return 666;
  }

  dstRect.x = gScreenWidth/2 - gameNameSurface->w/2;
  dstRect.y = 0;
  if(SDL_BlitSurface(gameNameSurface,NULL,gScreenSurface,&dstRect) < 0)
  {
    fprintf(stderr,"Impossivel blitar superfície do nome do jogo na tela!%s\n",SDL_GetError());
    //gGameStatus = -669;
    return 666;
  }

  dstRect.x = gScreenWidth/2 - oneSurface->w/2;
  dstRect.y = gScreenHeight / 2;
  if(SDL_BlitSurface(oneSurface,NULL,gScreenSurface,&dstRect) < 0)
  {
    fprintf(stderr,"Impossivel blitar opção 1 na tela!%s\n",SDL_GetError());
    //gGameStatus = -669;
    return 666;
  }

  dstRect.x = gScreenWidth/2 - twoSurface->w/2;
  dstRect.y = dstRect.y + 36;
  if(SDL_BlitSurface(twoSurface,NULL,gScreenSurface,&dstRect) < 0)
  {
    fprintf(stderr,"Impossivel blitar opção 2 na tela!%s\n",SDL_GetError());
    //gGameStatus = -669;
    return 666;
  }

  dstRect.x = gScreenWidth/2 - threeSurface->w/2;
  dstRect.y = dstRect.y + 36;
  if(SDL_BlitSurface(threeSurface,NULL,gScreenSurface,&dstRect) < 0)
  {
    fprintf(stderr,"Impossivel blitar opção 3 na tela!%s\n",SDL_GetError());
    //gGameStatus = -669;
    return 666;
  }



  while(!leave)
  {
    SDL_UpdateWindowSurface(gWindow);
    while(SDL_PollEvent(&event) != 0)
    {
      switch(event.type)
      {
        case SDL_KEYDOWN:
          if(event.key.keysym.sym == SDLK_1)
          {
            freeMenu();
            return 1;
          }
          if(event.key.keysym.sym == SDLK_2)
          {
            freeMenu();
            return 2;
          }
          if(event.key.keysym.sym == SDLK_3 || event.key.keysym.sym == SDLK_ESCAPE)
          {
            freeMenu();
            return 3;
          }
        case SDL_QUIT:
          return 3;
      }
    }
  }
}


void freeMenu()
{
  SDL_FreeSurface(gameNameSurface);
  SDL_FreeSurface(oneSurface);
  SDL_FreeSurface(twoSurface);
  SDL_FreeSurface(threeSurface);

  /* Tornando a superfície escura novamente */
  SDL_FillRect( gScreenSurface, NULL,
      SDL_MapRGB( gScreenSurface->format, 0, 0, 0 ) );
}


void showRanking()
{
	int i;
  /* poe o hiscore */
  gRank = fopen("./data/rank/rank.bin","rb");
  if(!gRank){
    puts("Impossivel abrir arquivo do rank!");
    gGameStatus = -666;
    return false;
  }

  readPlayers();
  int x;
  for (i = 0; i < 5; i++) {
    //char txtToRender[12];
	  sprintf(txtToRender,"%s: %d pts; %ld\n", gPlayers[i].name, gPlayers[i].pts, gPlayers[i].sysTime);
    if((x = renderAndBlit(i)) != 0)
    {
      printf("%d\n",x);
    }
  }
  SDL_UpdateWindowSurface(gWindow);
  SDL_Delay(10000);
  fclose(gRank);
  freeRanking(--i);
  switch (menu()) {
		case 2:
			showRanking();
			break;
		case 3:
			exit(3);
	}
}

int renderAndBlit(int i)
{
  SDL_Rect dstRect;
  if(!(bestPlayersSurface[i] = TTF_RenderText_Shaded(gScoreFonte,txtToRender,gScoreFontColor,gBgColor))) {
		fprintf(stderr,"Impossivel renderizar bestplayer %d na superfície!%s\n",i + 1,TTF_GetError());
		//gGameStatus = -101;
		return 11;
	}

  dstRect.x = gScreenWidth/2 - bestPlayersSurface[i]->w/2;
  dstRect.y =gScreenWidth / 4 + 36 * (i);

  if(SDL_BlitSurface(bestPlayersSurface[i],NULL,gScreenSurface,&dstRect) < 0)
  {
    fprintf(stderr,"Impossivel blitar texto de tryagain na tela!%s\n",SDL_GetError());
    //gGameStatus = -669;
    return 12;
  }
}

void freeRanking(int i)
{
  while(i > -1)
  {
    SDL_FreeSurface(bestPlayersSurface[i]);
    i--;
  }
}
