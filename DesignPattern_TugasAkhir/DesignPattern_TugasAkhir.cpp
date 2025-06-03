#include <raylib.h>
#include "Game.h"
#include "Class/UIManager/UIManager.h"



int main()
{
	Color WindowBackground = { 29,29,27,255 };
	int offset = 50;
	int WindowWidth = 750;
	int WindowHeight = 700;

	InitWindow(WindowWidth + offset, WindowHeight + offset * 2, "Projek Tugas Akhir Space Invaders");
	SetWindowIcon(LoadImage("Graphics/alien_3.png"));
	InitAudioDevice();

	Texture2D SpaceShipImage = LoadTexture("Graphics/spaceship.png");

	SetTargetFPS(60);

	Game game;
	
	while(WindowShouldClose() == false)
	{

		game.HandleInput();
		game.update();
		BeginDrawing();
		ClearBackground(WindowBackground);
		UIManager::GetInstance()->SetupUI();

		//This is more or less checking if the game still running or not 
		//based on the spaceship live if 0 it goes to Game Over if not then 
		//We run the UI at runtime with all the value from game
		if (game.run) {
			UIManager::GetInstance()->RuntimeUI();
			UIManager::GetInstance()->GameLevelNumber();
			UIManager::GetInstance()->SpaceShipHealth(SpaceShipImage);
		}
		else {
			UIManager::GetInstance()->GameOverUI();
		}
		
		UIManager::GetInstance()->GameScoreUI();
		
		game.Draw();		
		EndDrawing();
		
	}

	CloseWindow();
	CloseAudioDevice();
}