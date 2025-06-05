#pragma once
#include <raylib.h>
#include "UIObserverr.h"

class UIManager : public UIObserver {
    private:
        static UIManager* instance;
        UIManager() {
            fontPixel = LoadFontEx("Font/pixelated-futura (1).ttf", 64, 0, 0);
        }
        UIObserver* observer = nullptr;

        int lastScore = 0;
        int lastHighScore = 0;
        int lastLevel = 1;
        int lastLives = 3;


    public:
        static UIManager* GetInstance() {
            if (!instance)
                instance = new UIManager(); //lazy initialization
            return instance;
        }
        //UIObserver Subscribe system (its what im getting from how obersver work)
        void AttachObserver(UIObserver* UIobserver);
        void OnGameDataChanged(int score, int highScore, int level, int lives) override;

        void SetupUI();
        Font fontPixel;
        void UIUpdate();
        void RuntimeUI();
        void GameOverUI();
        void GameScoreUI();
        void GameLevelNumber();
        void SpaceShipHealth(Texture2D SpaceShipImage);
        int NumberLevel = 1;
    };