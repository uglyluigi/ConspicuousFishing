#ifndef UTILC
#define UTILC

#include <stdlib.h>
#include "pd_api/pd_api_gfx.h"
#include "vec.c"
#include "pd_api.h"

int rand_btwn(int nMin, int nMax) {
  return rand() % ((nMax + 1) - nMin) + nMin;
}

Vec2D* rand_pos() {
	Vec2D* pos = vec2d_new((float)rand_btwn(0, LCD_COLUMNS), (float)rand_btwn(0, LCD_ROWS));
	
	return pos;
}

#endif