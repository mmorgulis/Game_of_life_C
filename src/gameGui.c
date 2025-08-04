#include "gridView.h"
#include "life.h"
#include "gameGui.h"
#include <stdbool.h>
#include <raylib.h>

int main(void)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(500, 500, "Life");
	
	const unsigned int cellSize = 8; // default
	const unsigned int zoomLevels[] = {1, 2, 4, 8};
	const unsigned int zoomCount = sizeof(zoomLevels) / sizeof(zoomLevels[0]);
	unsigned int currentZoomIndex = 1; // parto da zoom =zoom = 22
	GridView gridView = {cellSize, 0 , 0, zoomLevels[currentZoomIndex]};
	
	World world = { 0 };
	world.gameSettings.paused = false;
	world.gameSettings.speed = 1;
	SetTargetFPS(world.gameSettings.speed * 10); // 1 speed

	unsigned int population = 5;
	unsigned int generation = 0;

	// Starting pattern
	createNewCell(&world, 15, 15);
	createNewCell(&world, 14, 15);
	createNewCell(&world, 15, 14);
	createNewCell(&world, 15, 16);
	createNewCell(&world, 16, 16);

	while (!WindowShouldClose())
	{	
		// Speed Button
		int width = GetScreenWidth();
		Rectangle speedDownBtn = { width - 50, 10, 25, 25 };
		Rectangle speedUpBtn = { width - 50, 39, 25, 25 };

		// Input with mouse = panning, zooming and create cells
		if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
			Vector2 delta = GetMouseDelta();
			gridView.shiftX += (int) delta.x;
			gridView.shiftY += (int) delta.y;
		}

		int scroll = GetMouseWheelMove();
		if (scroll > 0 && currentZoomIndex < zoomCount - 1) {
			currentZoomIndex++;
			gridView.zoom = zoomLevels[currentZoomIndex];
		}
		if (scroll < 0 && currentZoomIndex > 0) {
			currentZoomIndex--;
			gridView.zoom = zoomLevels[currentZoomIndex];
		}

		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			Vector2 position = GetMousePosition();

			if (CheckCollisionPointRec(position, speedDownBtn)) {
				if (world.gameSettings.speed > 1)
					world.gameSettings.speed--;
				SetTargetFPS(world.gameSettings.speed * 10);
			}
			else if (CheckCollisionPointRec(position, speedUpBtn)) {
				if (world.gameSettings.speed < 10)
					world.gameSettings.speed++;
				SetTargetFPS(world.gameSettings.speed * 10);
			}
			else {
				int cellX = (int)((position.x - gridView.shiftX) / (gridView.zoom * gridView.cellSize));
				int cellY = (int)((position.y - gridView.shiftY) / (gridView.zoom * gridView.cellSize));

				createNewCell(&world, cellX, cellY);
			}
		}

		

		DrawRectangleRec(speedDownBtn, GREEN);
		DrawText("-", speedDownBtn.x + 7, speedDownBtn.y, 30, WHITE);

		DrawRectangleRec(speedUpBtn, GREEN);
		DrawText("+", speedUpBtn.x + 5, speedUpBtn.y, 30, WHITE);
		

		BeginDrawing();
		ClearBackground(BLACK);
		gridViewDraw(&gridView);
		//cellEvolution(); // internally calls updateChunks
		drawChunks(&world, &gridView);
		DrawText(TextFormat("Population : %d", population), 15, 15, 19, GREEN);
		DrawText(TextFormat("Generation : %d", generation), 15, 46, 19, GREEN);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
