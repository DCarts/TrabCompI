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
int setClipboard(void); /* blita dinamicamente o nome do player na tela  */

void savePlayer(char*);/*  Cria e salva o player atual */

#endif
