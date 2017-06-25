/*
 * global.c
 * 
 * Copyright 2017 Daniel <dcsouza@dcc.ufrj.br>
 *                Guilherme <guiavenas@ufrj.br>
 *                Gabriel <gabrielizotongo@gmail.com>
 */

#include <math.h>
#include "defs.h"
#include "util.h"
#include "global.h"

double dotProduct(VETOR2D a, VETOR2D b) {
	return (a.x * b.x)+(a.y * b.y);
}

double normalize (VETOR2D* a) {
	double norm = sqrt((a->x * a->x) + (a->y * a->y));
	a->x = a->x / norm;
	a->y = a->y / norm;
	return norm;
}

double sqDist(VETOR2D a, VETOR2D b) {
	double dx, dy;
	dx = a.x - b.x;
	dy = a.y - b.y;
	return (dx*dx)+(dy*dy);
}

int isInAABB(VETOR2D t, double p1x, double p1y, double p4x, double p4y) {
	return     t.x < p4x
			&& t.x > p1x
			&& t.y < p4y
			&& t.y > p1y;
}

VETOR2D copyVector(VETOR2D orig) {
	VETOR2D novo;
	novo.x = orig.x;
	novo.y = orig.y;
	return novo;
}

int isInside(VETOR2D p, BLOCO* b) {
	return isInAABB(p, b->pos.x, 
					   b->pos.y,
					   b->pos.x + b->w,
					   b->pos.y + b->h);
}
