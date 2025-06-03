#pragma once
#include <raylib.h>
#include "../Command/ICommand.h"
#include "../Spaceship/SpaceShip.h"

class InputManager {
private:
    static InputManager* instance;
    InputManager() {} 
    ICommand* shootCommand = nullptr;


public:
    static InputManager* GetInstance() {
        if (!instance)
            instance = new InputManager(); //lazy initialization
        return instance;
    }

    void RegisterCommands(class SpaceShip* ship);//register all type of command if i want too
    void HandleInput(class SpaceShip& spaceship);
    void InputGameOver();
};
