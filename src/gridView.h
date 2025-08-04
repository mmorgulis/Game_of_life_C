#ifndef GRID_H
#define GRID_H

#include <stdbool.h>

typedef struct {
	int cellSize;
	int shiftX;
	int shiftY;
	int zoom;
} GridView;

void gridViewDraw(const GridView* gridView);

void cellDraw(int cellX, int cellY, const GridView* gridview);

#endif
