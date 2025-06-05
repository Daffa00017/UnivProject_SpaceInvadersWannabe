#include "InputManager.h"
#include "../UIManager/ShootLasersCommand.h"


InputManager* InputManager::instance = nullptr;


void InputManager::RegisterCommands(SpaceShip* ship)
{
    shootCommand = new ShootLaserCommand(ship);// for now we only doing the Laser one for command pattern hehe

}

void InputManager::HandleInput(SpaceShip& spaceship)
{
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        spaceship.MoveLeft();
    }
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        spaceship.MoveRight();
    }
    else if ((IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) && shootCommand) {
        shootCommand->Execute();
    }

}


void InputManager::InputGameOver()
{
}
