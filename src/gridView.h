#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <stdbool.h>

typedef struct {
	int cellSize;
	int shiftX;
	int shiftY;
	int zoom;
} GridView;

void drawGridView(const GridView* gridView);

void drawCell(int cellX, int cellY, const GridView* gridview);

#endif
