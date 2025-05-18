#include "Game.h"
#include "Class/InputManager/InputManager.h"
#include <iostream>
#include <fstream>

Game::Game()
{ 
	InitGame();
}

Game::~Game()
{
	Alien::UnloadImages();
}

void Game::update()
{
	if (run) {
		double currenttime = GetTime();
		if (currenttime - ufoLastSpawnTime > ufoSpawnInterval) {
			ufo.spawn();
			ufoLastSpawnTime = GetTime();
			ufoSpawnInterval = GetRandomValue(10, 20);
		}

		//To make the laser move update position.y
		for (auto& laser : SpaceShip.lasers) {
			laser.update();
		}

		MoveAliens();
		alienShootLaser();

		for (auto& laser : alienLaser) {
			laser.update();
		}

		DeleteInactiveLaser();
		//std::cout << "Vector size : " << SpaceShip.lasers.size() << std::endl;

		ufo.update();

		CheckForCollision();
	}
	else {
		if (IsKeyPressed(KEY_ENTER)) {
			Reset();
			InitGame();
		}
		 
	}
}

void Game::Draw()
{
	//to render the spaceship
	SpaceShip.Draw();

	//to draw or render the laser
	for (auto& laser : SpaceShip.lasers) {
		laser.Draw();
	}

	for (auto& obstacle : obstacles) {
		obstacle.Draw();
	}

	for (auto& alien : aliens) {
		alien.Draw();
	}

	for (auto& laser : alienLaser) {
		laser.Draw();
	}

	ufo.draw();
}



void Game::HandleInput()
{
	if (run) 
	{
		InputManager::GetInstance()->HandleInput(SpaceShip);
		/*if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
		{
			SpaceShip.MoveLeft();
		}
		else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
			SpaceShip.MoveRight();
		}
		else if (IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			SpaceShip.ShootLaser();
		}*/
	}

}

void Game::DeleteInactiveLaser()
{
	//Check if the laser is active or not if it not active we erase it
	for (auto it = SpaceShip.lasers.begin(); it != SpaceShip.lasers.end();) {
		if (!it->active) {
			it = SpaceShip.lasers.erase(it);
		}
		else {
			++it;
		}
	}
	for (auto it = alienLaser.begin(); it != alienLaser.end();) {
		if (!it->active) {
			it = alienLaser.erase(it);
		}
		else {
			++it;
		}
	}
}

std::vector<Obstacle> Game::CreateObstacles()
{
	int obstacleWidth = Obstacle::grid[0].size() * 3;
	float gap = (GetScreenWidth() - (4 * obstacleWidth)) / 5;

	for (int i = 0; i < 4; i++) {
		float offsetX = (i + 1) * gap + i * obstacleWidth;
		obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight()-200)} ) );
	}
	return obstacles;
}

std::vector<Alien> Game::CreateAliens()
{
	std::vector<Alien> aliens;
	for (int row = 0; row < 5; row++) {
		for (int column = 0; column < 11; column++) {
			//Check the row of the alien from up to apply alien type
			int alientype;
				if (row == 0) {
					alientype = 3;
				}
				else if (row == 1 || row == 2) {
					alientype = 2;
				}
				else {
					alientype = 1;
				}	


			float x = 75 + column * cellsize;
			float y = 110 + row * cellsize;
			aliens.push_back(Alien(alientype, { x,y }));
		}
	}
	return aliens;
}
 
void Game::MoveAliens()
{
	for (auto& alien : aliens) {
		if (alien.position.x + alien.alienImages[alien.type - 1].width > GetScreenWidth() - 25){
			aliensdirection = -1;
			MoveDownAliens(aliensDownPixel);
		}
		if (alien.position.x < 25) {
			aliensdirection = 1;
			MoveDownAliens(aliensDownPixel);
		}

		alien.Update(aliensdirection);

	}
}

void Game::MoveDownAliens(int distance)
{
	for (auto& alien:aliens) {
		alien.position.y += distance; 
	}
}

void Game::alienShootLaser()
{
	double currenttime = GetTime();
	if(currenttime - timeLastAlienLaser >= alienLaserInterval && !aliens.empty()){
		int randomIndex = GetRandomValue(0, aliens.size() - 1);
		Alien& alien = aliens[randomIndex];
		alienLaser.push_back(Laser({ alien.position.x + alien.alienImages[alien.type - 1].width / 2, alien.position.y + alien.alienImages[alien.type - 1].height }, 6));
		timeLastAlienLaser = GetTime();
	}
}

void Game::CheckForCollision()
{
	//SpaceShip Laser with aliens and obstacle
	for (auto& Laser : SpaceShip.lasers) 
	{
		auto it = aliens.begin();
		while (it != aliens.end()) {
			if (CheckCollisionRecs(it->getRect(), Laser.getRect())) 
			{
				if (it->type == 1) {
					score += 100;
				}else if (it->type == 2) {
					score += 200;
				}
				else if (it->type == 3) {
					score += 300;
				}
				CheckForHighscore();

				it = aliens.erase(it); //If hit alien, alien gone
				Laser.active = false;
			}
			else {
				++it;
			}
		}

		for (auto& obstacles : obstacles) 
		{
			auto it = obstacles.blocks.begin();
			while (it != obstacles.blocks.end()) {
				if (CheckCollisionRecs(it -> getRect(), Laser.getRect())) {
					it = obstacles.blocks.erase(it);
					Laser.active = false;
				}
				else {
					++it;
				}
			}
		}

		if (CheckCollisionRecs(ufo.getRect(), Laser.getRect())) {
			ufo.alive = false;
			Laser.active = false;
			score += 500;
			CheckForHighscore();
		}
	}

	//Alien Laser with spaceship and obstacle

	for (auto& Laser : alienLaser) {
		if (CheckCollisionRecs(Laser.getRect(), SpaceShip.getRect())) {
			Laser.active = false;
			//std::cout << "SpaceShip Getting Hit" << std::endl;
			lives--;
			if (lives <= 0) {
				GameOver();
			}
		}

		for (auto& obstacles : obstacles)
		{
			auto it = obstacles.blocks.begin();
			while (it != obstacles.blocks.end()) {
				if (CheckCollisionRecs(it->getRect(), Laser.getRect())) {
					it = obstacles.blocks.erase(it);
					Laser.active = false;
				}
				else {
					++it;
				}
			}
		}
	}

	//Alien Collision with obstacle
	for (auto& alien : aliens) {
		for (auto& Obstacle : obstacles) {
			auto it = Obstacle.blocks.begin();
			while (it != Obstacle.blocks.end()) {
				if (CheckCollisionRecs(it->getRect(), alien.getRect())) {
					it = Obstacle.blocks.erase(it);
				}
				else {
					it++;
				}
			}
		}

		if (CheckCollisionRecs(alien.getRect(), SpaceShip.getRect())) {
			//std::cout << "SpaceShip hit by aliens" << std::endl;
			GameOver();
		}
	}
}

void Game::GameOver()
{
	std::cout << "Game Over" << std::endl;
	run = false;

}

void Game::Reset()
{
	SpaceShip.Reset();
	aliens.clear();
	alienLaser.clear();
	obstacles.clear();
}

void Game::InitGame()
{
	//obstaclesetup
	obstacles = CreateObstacles();
	//int cellsize = 55;
	//AlienSetup
	aliens = CreateAliens();
	aliensdirection = 1;
	timeLastAlienLaser = 0.0;
	//UfoSetup
	ufoLastSpawnTime = 0.0;
	ufoSpawnInterval = GetRandomValue(10, 20);
	run = true;
	lives = 3;
	score = 0;
	Highscore = loadHighScoreFromFile();
}

void Game::CheckForHighscore()
{
	if (score > Highscore) {
		Highscore = score;
		SavehighScoreToFile(Highscore);
	}
}

void Game::SavehighScoreToFile(int highscore)
{
	std::ofstream highscoreFile("HighScore.txt");
	if (highscoreFile.is_open()) {
		highscoreFile << highscore;
		highscoreFile.close();
	}
	else {
		std::cerr << "failed to Save highscore failed to File" << std::endl;
	}
}

int Game::loadHighScoreFromFile()
{
	int loadedHighScore = 0;
	std::ifstream highscoreFile("HighScore.txt");
	if (highscoreFile.is_open()) {
		highscoreFile >> loadedHighScore;
		highscoreFile.close();
	}
	else {
		std::cerr << "Failed to load Highscore from file" << std::endl;
	}
	return  loadedHighScore;
}

