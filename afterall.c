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

 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
 #include <SDL2/SDL_mixer.h>
 #include <SDL2/SDL_ttf.h>

 #include "defs.h"
 #include "global.h"
 #include "render.h"
 #include "afterall.h"

static char standardMessage[22] = "Enter your name:";
static char namae[22] = "ze sa";
static SDL_Surface* standardMSurface = NULL;
static SDL_Surface* clipboardSurface = NULL;
static SDL_Event ev;
static SDL_Rect dstRect;


int setClipboard(){	/* função para capturar a entrada do nome do player e blitar diamicamente na tela */
	int exit = false;
  int err = false, cont = strlen(namae);


	/* Tornando a superfície escura novamente */
	SDL_FillRect( gScreenSurface, NULL,
	SDL_MapRGB( gScreenSurface->format,
				0, 0, 0 ) );

	/*	renderizando a mensagem padrão na superfície	*/
	if(!(standardMSurface = TTF_RenderText_Shaded(gScoreFonte,standardMessage,gScoreFontColor,gBgColor))){
		fprintf(stderr,"Impossivel renderizar texto standardMessage na tela!%s\n",TTF_GetError());
		err = true;
	}
	else{	printf("Renderizei ok\n");}

	dstRect.x = 250; dstRect.y = 0;

	/*	blitando a superfície na tela	*/
	if (SDL_BlitSurface(standardMSurface, NULL, gScreenSurface,&dstRect) < 0) {
		fprintf(stderr,"Impossivel blitar texto padrão na tela! %s\n",SDL_GetError());
		err = true;
	}
	else{	printf("Blitei ok\n");}

	SDL_StartTextInput();	/* habilitando a entrada de texto pelo usuário	*/

	dstRect.x = 330; dstRect.y = 200;	/*	definindo a posição do retangulo do nome do jogador	*/
	while(!exit){
		int ableRender = true;

    while(SDL_PollEvent(&ev) != 0){

  		/*printf("Entrei\n");*/


  		if(ev.type == SDL_KEYDOWN){
  			/*if(ev.key.keysym.sym == SDLK_BACKSPACE && cont > 0){

  				namae[cont--] = ' ';
  				ableRender = true;

  			}
  			else if(!( ( ev.text.text[ 0 ] == 'c' || ev.text.text[ 0 ] == 'C' ) &&
  								( ev.text.text[ 0 ] == 'v' || ev.text.text[ 0 ] == 'V' ) &&
  								SDL_GetModState() & KMOD_CTRL )){

  				namae[cont++] = ev.text.text[0];
  				ableRender = true;
  			}*/
  			if(ev.key.keysym.sym == SDLK_RETURN || ev.key.keysym.sym == SDLK_RETURN2){
  				printf("p\n");
  				break;
  			}

  		}
  		else if(ev.type == SDL_TEXTINPUT){

  			if( !( ( ev.text.text[ 0 ] == 'c' || ev.text.text[ 0 ] == 'C' ) && ( ev.text.text[ 0 ] == 'v' || ev.text.text[ 0 ] == 'V' ) && 						SDL_GetModState() & KMOD_CTRL ) ){
  							/*	adiciona o caracter ao fim da string	*/
  							namae[++cont] = ev.text.text[0];
  							ableRender = true;

  						}
  		}

  		if(ableRender){/* literalmente a pegunta "posso renderizar o texto?"*/

  			/*	Renderizando o texto de entrada na superfície	*/
  			if(!(clipboardSurface = TTF_RenderText_Shaded(gScoreFonte,namae,gScoreFontColor,gBgColor))){
  				fprintf(stderr,"Impossivel renderizar texto de entrada na tela!%s\n",TTF_GetError());
  				err = true;
  				break;
  			}
  			/*else{	printf("Renderizei ok\n");}*/
  		}

  		/*	blitando a superfície na tela	*/
  		if (SDL_BlitSurface(clipboardSurface, NULL, gScreenSurface, &dstRect) < 0) {
  			fprintf(stderr,"Impossivel blitar texto de entrada na tela! %s\n",SDL_GetError());
  			err = true;
  			break;
  		}
  		/*else{	printf("Blitei ok\n");}*/

  		SDL_UpdateWindowSurface(gWindow);
  		/*printf("%s\n",namae);*/
  	}

	}


	SDL_StopTextInput(); /*	encerrando a entrada de texto	*/

	/*SDL_Delay(5000); teste para ver se blitou,será removido na versão final.	*/

	if(err){
		printf("Erro ao renderizar clipboard\n");
		return 0;
	}

  return 1;
}


void createPlayer(){
	char buffer[22] = "abba";

	while(true){	/*permanecer na função até que o usuário nao digite merda(digite algum caracter)*/
		//printf("Qual o nome do jogador? \n");
		//fgets(buffer,22,stdin);
		if (/*sscanf(buffer, "%s",gPlayer.nome)*/1 != EOF) {
			strcpy(gPlayer.nome, /*buffer*/"as");
			break;
		}
	}

		gPlayer.nome[strlen(gPlayer.nome) - 1] = '\0';
		strcat(buffer/*gPlayer.nome*/,"  ");	/* append dois espaços	*/
		strcat(buffer/*gPlayer.nome*/,gScoreText);	/* append pontuação do jogador */
		strcat(buffer,"\n"); /*	append '\n'	*/

		gRank = fopen("./bin/data/rank/rank.txt","a+");
		if(!gRank){
			puts("Impossível abrir arquivo do rank!");
			exit(666);
		}

		fputs(buffer,gRank);	/*	grava o nome do jogador no arquivo apontado por gRank	*/


		fclose(gRank);
		gPlayer.vidas = 5;
		gPlayer.pontos = 0;
}
