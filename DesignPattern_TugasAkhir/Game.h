#pragma once
#include "Class/Spaceship/SpaceShip.h"
#include "Class/Obstacle/Obstacle.h"
#include "Class/Alien/Alien.h"

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
		std::vector<Alien> CreateAliens();
		void MoveAliens();
		void MoveDownAliens(int distance);
		void alienShootLaser();
		SpaceShip SpaceShip;
		std::vector<Obstacle> obstacles;
		std::vector<Alien> aliens;
		int cellsize = 55;
		int aliensdirection;
		int aliensDownPixel = 4;
		std::vector<Laser> alienLaser;
		constexpr static float alienLaserInterval = 0.35;
		float timeLastAlienLaser;
};
