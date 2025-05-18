#include <raylib.h>
#include "Game.h"

int main()
{
	Color WindowBackground = { 29,29,27,255 };
	int offset = 50;
	int WindowWidth = 750;
	int WindowHeight = 700;

	InitWindow(WindowWidth + offset, WindowHeight + offset * 2, "Projek Tugas Akhir Space Invaders");

	Font font = LoadFontEx("Font/pixelated-futura (1)", 64, 0, 0);
	Texture2D SpaceShipImage = LoadTexture("Graphics/spaceship.png");

	SetTargetFPS(60);

	Game game;
	
	while(WindowShouldClose() == false)
	{

		game.HandleInput();
		game.update();
		BeginDrawing();
		ClearBackground(WindowBackground);
		DrawRectangleRoundedLinesEx({ 10, 10, 780,780 }, 0.18f, 20, 2, YELLOW);
		DrawLineEx({ 25, 730 }, { 775, 730 }, 3, YELLOW);

		//Add text to level and check if the player death or no
		if (game.run) {
			DrawTextEx(font, "Level 01", { 570,740 }, 34, 2, YELLOW);
		}
		else {
			DrawTextEx(font, "Game Over", { 100,740 }, 34, 2, YELLOW);
			DrawTextEx(font, "[Enter] To Restart", { 440,740 }, 34, 2, YELLOW);
		}
		float x = 50;
		for (int i = 0; i < game.lives; i++) {
			DrawTextureV(SpaceShipImage, { x, 745 }, WHITE);
			x += 50;
		}

		game.Draw();		
		
		EndDrawing();
		
	}

	CloseWindow();
}