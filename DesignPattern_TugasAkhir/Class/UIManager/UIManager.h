#pragma once
#include <raylib.h>

class UIManager {
private:
    static UIManager* instance;
    UIManager() {}

public:
    static UIManager* GetInstance() {
        if (!instance)
            instance = new UIManager(); //lazy initialization
        return instance;
    }

    void SetupUI();
    Font fontPixel = LoadFontEx("Font/pixelated-futura (1).ttf", 64, 0, 0);
    void UIUpdate();
    void RuntimeUI();
    void GameOverUI();
    void GameScoreUI(int Score, int HighScore);
    void GameLevelNumber(int NumberLevel);
    void SpaceShipHealth(int lives, Texture2D SpaceShipImage);
    int NumberLevel = 1;

};