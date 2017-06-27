/*
 * defs.h
 *
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */


 #ifndef AFTERALL_H
 #define AFTERALL_H

 /*retorna 0 se tiver erro,retorna 1 se for tudo ok*/
 /*parametro gameOver: se o jogo acabou pq o player saiu ou porque as vidas acabaram
  * 1-acabaram as vidas (renderiza "Game Over!")
  * 0-nao acabaram as vidas, o jogador saiu por vontade propria (soh salva o score)*/
int setClipboard(int gameOver); 

/*  Cria e salva o player atual, se ele está no top 5 dos scores salvos */
void savePlayer(char*);

/* sorta os scores pra salvar */
int sortByScore(const void* a, const void* b);

/* blita a mensagem de tryagain e recomeça o jogo */
/*	retorna != 0 se der ruim,retorna 0 se for de boas*/
int tryAgain(void);

/*	libera as superfícies usadas em tryagain 	*/
void freeTryAgain(void);

#endif
