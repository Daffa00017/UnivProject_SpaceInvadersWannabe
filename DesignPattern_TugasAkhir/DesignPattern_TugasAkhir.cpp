#include <raylib.h>
#include "Game.h"
#include "Class/UIManager/UIManager.h"
#include <string>



int main()
{
	Color WindowBackground = { 29,29,27,255 };
	int offset = 50;
	int WindowWidth = 750;
	int WindowHeight = 700;

	InitWindow(WindowWidth + offset, WindowHeight + offset * 2, "Projek Tugas Akhir Space Invaders");
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

		//Add text to level and check if the player death or no
		if (game.run) {
			UIManager::GetInstance()->RuntimeUI();
			UIManager::GetInstance()->GameLevelNumber(game.NumberOfLevel);
			UIManager::GetInstance()->SpaceShipHealth(game.lives, SpaceShipImage);
		}
		else {
			UIManager::GetInstance()->GameOverUI();
		}
		
		UIManager::GetInstance()->GameScoreUI(game.score, game.Highscore);
		
		game.Draw();		
		EndDrawing();
		
	}

	CloseWindow();
	CloseAudioDevice();
}