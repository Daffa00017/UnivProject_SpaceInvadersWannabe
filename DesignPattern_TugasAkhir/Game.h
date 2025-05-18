#pragma once
#include "Class/Spaceship/SpaceShip.h"
#include "Class/Obstacle/Obstacle.h"
#include "Class/Alien/Alien.h"
#include "Class/Alien/UFO.h"

class Game {
	public:
		Game();
		~Game();
		void Draw();
		void update();
		void HandleInput();
		int lives = 3;
		int score;
		int Highscore;
		bool run;

	private:	
		void DeleteInactiveLaser();
		std::vector<Obstacle> CreateObstacles();
		std::vector<Alien> CreateAliens();
		void MoveAliens();
		void MoveDownAliens(int distance);
		void alienShootLaser();
		void CheckForCollision();
		void GameOver();
		void Reset();
		void InitGame();
		void CheckForHighscore();
		void SavehighScoreToFile(int highscore);
		int loadHighScoreFromFile();
		SpaceShip SpaceShip;
		std::vector<Obstacle> obstacles;
		std::vector<Alien> aliens;
		int cellsize = 55;
		int aliensdirection;
		int aliensDownPixel = 4;
		std::vector<Laser> alienLaser;
		constexpr static float alienLaserInterval = 0.35;
		float timeLastAlienLaser;
		Ufo ufo;
		float ufoSpawnInterval;
		float ufoLastSpawnTime;
};
