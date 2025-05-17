#include <raylib.h>
#include "Game.h"

int main()
{
	Color WindowBackground = { 29,29,27,255 };
	int WindowWidth = 750;
	int WindowHeight = 700;

	InitWindow(WindowWidth, WindowHeight, "Projek Tugas Akhir Space Invaders");
	SetTargetFPS(60);

	Game game;
	
	while(WindowShouldClose() == false)
	{

		game.HandleInput();
		game.update();
		BeginDrawing();
		ClearBackground(WindowBackground);
		game.Draw();		
		
		EndDrawing();
		
	}

	CloseWindow();
}