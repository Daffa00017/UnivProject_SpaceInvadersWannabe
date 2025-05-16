#include <raylib.h>
#include "Class/Spaceship/SpaceShip.h"

int main()
{
	Color WindowBackground = { 29,29,27,255 };
	int WindowWidth = 750;
	int WindowHeight = 700;

	InitWindow(WindowWidth, WindowHeight, "Projek Tugas Akhir Space Invaders");
	SetTargetFPS(60);

	SpaceShip spaceship;

	while(WindowShouldClose() == false)
	{
		BeginDrawing();
		ClearBackground(WindowBackground);
		spaceship.Draw();

		EndDrawing();
		
	}

	CloseWindow();
}