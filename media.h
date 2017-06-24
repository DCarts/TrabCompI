/*
 * defs.h
 * 
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */

#ifndef MEDIADEFS_H
#define MEDIADEFS_H

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

/* Carrega as imagens e audio */
int loadMedia();

/* Carrega uma imagem */
SDL_Surface* loadSurface (char* path);

/* Carrega um som */
Mix_Chunk* loadSound(char* path);

/* Carrega uma fonte */
TTF_Font* loadFont(char* path);

/* Carrega blocos de um level *
 * O level deve estar em ./data/level/ *
 * Retorna true se leu de boas, false se deu merda */
int loadBlocosFromFile(char* levelName);

#endif
