#pragma once
#include "Class/Spaceship/SpaceShip.h"

class Game {
	public:
		Game();
		~Game();
		void Draw();
		void update();
		void HandleInput();

	private:	
		void DeleteInactiveLaser();
		SpaceShip SpaceShip;
};
