#include "gridView.h"
#include <stdbool.h>
#include "raylib.h"

void gridViewDraw(const GridView* gridView) {
	int screenW = GetScreenWidth();
	int screenH = GetScreenHeight();
	int cellSize = (int) gridView->cellSize * gridView->zoom;
	int shiftX = gridView->shiftX;
	int shiftY = gridView->shiftY;

	for (int x = shiftX % cellSize; x < screenW; x += cellSize) {
		DrawLine(x, 0, x, screenH, LIGHTGRAY);
	}
	
	for (int y = shiftY % cellSize; y < screenH; y += cellSize) {
		DrawLine(0, y, screenW, y, LIGHTGRAY);
	}
	
}

void cellDraw(int cellX, int cellY, const GridView* gridview) {
	int size = gridview->zoom * gridview->cellSize;

	int screenX = gridview->shiftX + cellX * size;
	int screenY = gridview->shiftY + cellY * size;

	DrawRectangle(screenX, screenY, size, size, GREEN);
	//TraceLog(LOG_INFO, "ScreenX/Y %d %d", screenX, screenY);
}
