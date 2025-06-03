#pragma once
#include "../Command/ICommand.h"
#include "../Spaceship/SpaceShip.h"

class ShootLaserCommand : public ICommand 
{
private :
	SpaceShip* spaceship;
public:
    ShootLaserCommand(SpaceShip* spaceship) : spaceship(spaceship) {}
    void Execute() override {
        spaceship->ShootLaser();
    }

};

