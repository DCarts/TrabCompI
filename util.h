/*
 * defs.h
 * 
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */

#ifndef GAMEUTIL_H
#define GAMEUTIL_H
 
#include "defs.h"
 
double normalize (VETOR2D* a);
double dotProduct(VETOR2D a, VETOR2D b);
double sqDist (VETOR2D a, VETOR2D b);
int isInAABB(VETOR2D t, double p1x, double p1y, double p4x, double p4y);
VETOR2D copyVector(VETOR2D orig);
int isInside(VETOR2D p, BLOCO* b);
void turnRad(VETOR2D* v, double angle);

#endif
