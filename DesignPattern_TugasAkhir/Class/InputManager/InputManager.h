#pragma once
#include <raylib.h>
#include "../Spaceship/SpaceShip.h"

class InputManager {
private:
    static InputManager* instance;
    InputManager() {} 

public:
    static InputManager* GetInstance() {
        if (!instance)
            instance = new InputManager(); //lazy initialization
        return instance;
    }

    void HandleInput(class SpaceShip& spaceship);
    void InputGameOver();
};
