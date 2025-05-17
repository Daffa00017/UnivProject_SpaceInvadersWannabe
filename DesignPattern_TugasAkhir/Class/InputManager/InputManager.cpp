#include "InputManager.h"

InputManager* InputManager::instance = nullptr;

void InputManager::HandleInput(SpaceShip& spaceship)
{
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        spaceship.MoveLeft();
    }
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        spaceship.MoveRight();
    }
    else if (IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        spaceship.ShootLaser();
    }

}
