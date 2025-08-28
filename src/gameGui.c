#include "gridView.h"
#include "life.h"
#include <stdbool.h>
#include <raylib.h>

int main(void)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 650, "Life");

	Font retroFont = LoadFont("resources/PressStart2P-Regular.ttf");
	if (retroFont.texture.id == 0) {
		TraceLog(LOG_INFO, "Font non trovato!");
	}

	const unsigned int cellSize = 8; // default
	const unsigned int zoomLevels[] = { 1, 2, 4, 8 };
	const unsigned int zoomCount = sizeof(zoomLevels) / sizeof(zoomLevels[0]);
	unsigned int currentZoomIndex = 1; // parto da zoom = 2
	GridView gridView = { cellSize, 0 , 0, zoomLevels[currentZoomIndex] };

	World world = { 0 };
	world.gameSettings.paused = true;
	world.gameSettings.gameStarter = false;
	world.gameSettings.speed = 1;
	SetTargetFPS(world.gameSettings.speed * 10); // 1 speed

	unsigned int generation = 0;

	// Starting pattern
	int center_x = (int) GetScreenWidth() / (2 * cellSize * zoomLevels[currentZoomIndex]);
	int center_y = (int) GetScreenHeight() / (2 * cellSize * zoomLevels[currentZoomIndex]);
	createNewCell(&world, center_x-1, center_y);
	createNewCell(&world, center_x, center_y-1);
	createNewCell(&world, center_x, center_y);
	createNewCell(&world, center_x, center_y+1);
	createNewCell(&world, center_x+1, center_y+1);

	while (!WindowShouldClose())
	{
		double tempo = GetTime();
		BeginDrawing();
		ClearBackground(BLACK);

		if (!world.gameSettings.gameStarter) {
			// If any key is pressed the game starts
			if (GetKeyPressed() != 0 || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				world.gameSettings.gameStarter = true;
				world.gameSettings.paused = false;
			}
			// Text and properties
			const char* title = "Conway's Game of Life";
			const char* instructions[] = {
				"Press SPACE to pause/resume",
				"Left Mouse: draw cells (better when paused)",
				"Right Mouse: pan view",
				"Mouse Wheel: zoom IN and OUT",
				"+ or - buttons: adjust speed",
				"Press ANY KEY to start"
			};

			float titleSize = 35;
			float instructionSize = 17;
			float startSize = 20;
			float spacing = 1;
			float titleSpacing = 20;
			float anySpacing = 20;

			// Measuere title
			Vector2 titleMeasure = MeasureTextEx(retroFont, title, titleSize, spacing);

			// Draw title
			float titleX = (GetScreenWidth() - titleMeasure.x) / 2;
			DrawTextEx(retroFont, title, (Vector2) { titleX, 15 }, titleSize, spacing, GREEN);

			// Middle instructions
			float startY = 15 + titleMeasure.y + 20; // 
			for (int i = 0; i < 5; i++) {
				Vector2 textMeasure = MeasureTextEx(retroFont, instructions[i], instructionSize, spacing);
				float textX = (GetScreenWidth() - textMeasure.x) / 2; // Centrato
				DrawTextEx(retroFont, instructions[i], (Vector2) { textX, startY + i * 25 }, instructionSize, spacing, GREEN);
			}

			// Final line
			Vector2 startMeasure = MeasureTextEx(retroFont, instructions[5], startSize, spacing);
			float startX = (GetScreenWidth() - startMeasure.x) / 2;
			DrawTextEx(retroFont, instructions[5], (Vector2) { startX, startY + 5 * 25 + anySpacing}, startSize, spacing, GREEN);
		}
		else {
			// Speed Button
			int width = GetScreenWidth();
			Rectangle speedDownBtn = { (float) width - 50.0f, 10.0f, 25.0f, 25.0f };
			Rectangle speedUpBtn = { width - 50.0f, 39.0f, 25.0f, 25.0f };

			// Input with mouse = panning, zooming and create cells
			if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
				Vector2 delta = GetMouseDelta();
				gridView.shiftX += (int)delta.x;
				gridView.shiftY += (int)delta.y;
			}

			int scroll = (int)GetMouseWheelMove();
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

			if (IsKeyPressed(KEY_SPACE)) {
				world.gameSettings.paused = !world.gameSettings.paused;
			}

			DrawRectangleRec(speedDownBtn, GREEN);
			DrawText("-", (int)speedDownBtn.x + 7, (int)speedDownBtn.y, 30, WHITE);

			DrawRectangleRec(speedUpBtn, GREEN);
			DrawText("+", (int)speedUpBtn.x + 5, (int)speedUpBtn.y, 30, WHITE);

			if (!world.gameSettings.paused) {
				updateWorld(&world);
				generation++;
			}

			drawGridView(&gridView);
			drawChunks(&world, &gridView);
			DrawText(TextFormat("Population : %d", world.cellCounter), 15, 15, 19, GREEN);
			DrawText(TextFormat("Generation : %d", generation), 15, 46, 19, GREEN);
		}
		EndDrawing();
	}

	resetWorld(&world);
	CloseWindow();

	return 0;
}
