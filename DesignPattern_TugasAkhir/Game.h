#pragma once
#include "Class/Spaceship/SpaceShip.h"
#include "Class/Obstacle/Obstacle.h"

class Game {
	public:
		Game();
		~Game();
		void Draw();
		void update();
		void HandleInput();

	private:	
		void DeleteInactiveLaser();
		std::vector<Obstacle> CreateObstacles();
		SpaceShip SpaceShip;
		std::vector<Obstacle> obstacles;
};
