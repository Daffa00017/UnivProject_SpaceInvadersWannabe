#include <raylib.h>
#include "Game.h"
#include "Class/UIManager/UIManager.h"

int main()
{
    Color WindowBackground = { 29,29,27,255 };
    int offset = 50;
    int WindowWidth = 750;
    int WindowHeight = 700;

    InitWindow(WindowWidth + offset, WindowHeight + offset * 2, "Trash Invaders");
    SetExitKey(KEY_NULL); // ESC won't insta-exit
    SetWindowIcon(LoadImage("Graphics/alien_3.png"));
    InitAudioDevice();

    Texture2D SpaceShipImage = LoadTexture("Graphics/spaceship.png");
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose() && !game.ShouldQuit())
    {
        game.HandleInput();
        game.update();

        BeginDrawing();
        ClearBackground(WindowBackground);

        UIManager::GetInstance()->SetupUI();
        using GS = Game::GameState;

       
        switch (game.GetState())
        {
            case GS::MainMenu:
               
                break;

            case GS::Playing:
            case GS::Paused:
                UIManager::GetInstance()->RuntimeUI();
                UIManager::GetInstance()->GameLevelNumber();
                UIManager::GetInstance()->SpaceShipHealth(game.GetPlayerShipTexture());
                UIManager::GetInstance()->GameScoreUI();
                break;

            case GS::GameOver:
                UIManager::GetInstance()->GameOverUI();
                UIManager::GetInstance()->GameScoreUI(); 
                break;
        }

        game.Draw();
        EndDrawing();
    }

    game.SaveOnExit();
    CloseWindow();
    CloseAudioDevice();
}
