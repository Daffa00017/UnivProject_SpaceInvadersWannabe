#include "Game.h"
#include "Class/InputManager/InputManager.h"
#include "Class/SoundManager/SoundManager.h"
#include "Class/UIManager/UIManager.h"
#include "Class/Alien/AlienFactory.h"
#include <iostream>
#include <fstream>

Game::Game()
{ 
	InputManager::GetInstance()->RegisterCommands(&SpaceShip); //register command pattern 
	Highscore = 0;
	NotifyUI();
	InitGame();
}

Game::~Game()
{
	Alien::UnloadImages();
	SoundManager::GetInstance()->unloadSounds();
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

		//To make the Laser move update position.y
		for (auto& Laser : SpaceShip.Lasers) {
			Laser.update();
		}

		MoveAliens();
		alienShootLaser();

		for (auto& Laser : alienLaser) {
			Laser.update();
		}

		DeleteInactiveLaser();
		//std::cout << "Vector size : " << SpaceShip.Lasers.size() << std::endl;

		ufo.update();

		CheckForCollision();

		//aliens empty = next level
		if (aliens.empty()) {
			//TraceLog(LOG_INFO, "No More Aliens"); // trying out the raylib version of print string in unreal engine
			NextLevel();
		}


	}
	else {
		if (IsKeyDown(KEY_ENTER)) {
			Reset();
			InitGame();
		}
		 
	}
}

void Game::Draw()
{
	//to render the spaceship
	SpaceShip.Draw();

	//to draw or render the Laser
	for (auto& Laser : SpaceShip.Lasers) {
		Laser.Draw();
	}

	for (auto& obstacle : obstacles) {
		obstacle.Draw();
	}

	for (auto& alien : aliens) {
		alien.Draw();
	}

	for (auto& Laser : alienLaser) {
		Laser.Draw();
	}

	ufo.draw();
}



void Game::HandleInput()
{
	if (run) 
	{
		InputManager::GetInstance()->HandleInput(SpaceShip);
	}

}

void Game::DeleteInactiveLaser()
{
	//Check if the Laser is active or not if it not active we erase it
	for (auto it = SpaceShip.Lasers.begin(); it != SpaceShip.Lasers.end();) {
		if (!it->active) {
			it = SpaceShip.Lasers.erase(it);
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

void Game::NotifyUI()
{
	UIManager::GetInstance()->OnGameDataChanged(score, Highscore, NumberOfLevel, lives);
}

std::vector<Obstacle> Game::CreateObstacles()
{
	int obstacleWidth = Obstacle::grid[0].size() * 3;
	float gap = (GetScreenWidth() - (4 * obstacleWidth)) / 5;

	for (int i = 0; i < 4; i++) {
		float offsetX = (i + 1) * gap + i * obstacleWidth;
		obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight()-250)} ) );
	}
	return obstacles;
}

std::vector<Alien> Game::CreateAliens()
{
	std::vector<Alien> aliens;
	for (int row = 0; row < 5; row++) {
		for (int column = 0; column < 11; column++) {
			int alientype = (row == 0) ? 3 : (row == 1 || row == 2) ? 2 : 1;
			float x = 75 + column * cellsize;
			float y = alienLocationDiff + 110 + row * cellsize;

			// Use the factory to create aliens with appropriate health
			Alien* alien = AlienFactory::CreateAlien(alientype, { x, y });
			aliens.push_back(*alien); // Add the created alien to the vector
			delete alien; // Delete the pointer since we copied it into the vector
		}
	}
	return aliens;
}
 
void Game::MoveAliens()
{
	for (auto& alien : aliens) {
		if (alien.position.x + alien.alienImages[alien.GetType() - 1].width > GetScreenWidth() - 25)
		{
			aliensdirection = -1 ;
			MoveDownAliens(aliensDownPixel);
		}
		if (alien.position.x < 25) {
			aliensdirection = 1 ;
			MoveDownAliens(aliensDownPixel);
		}

		alien.Update(aliensdirection);
		

	}
}

void Game::MoveDownAliens(int distance)
{
	for (auto& alien:aliens) {
		alien.position.y += distance ;
		if (alien.position.y + alien.alienImages[alien.GetType() - 1].height > GetScreenHeight() - 25) {
			GameOver();
		}
		
	}
}

void Game::alienShootLaser()
{
	double currenttime = GetTime();
	if(currenttime - timeLastAlienLaser >= alienLaserInterval && !aliens.empty()){
		int randomIndex = GetRandomValue(0, aliens.size() - 1);
		Alien& alien = aliens[randomIndex];
		alienLaser.push_back(Laser({ alien.position.x + alien.alienImages[alien.GetType() - 1].width / 2, alien.position.y + alien.alienImages[alien.GetType() - 1].height }, 6));
		timeLastAlienLaser = GetTime();
	}
}

void Game::CheckForCollision()
{
	//SpaceShip Laser with aliens and obstacle
	for (auto& Laser : SpaceShip.Lasers)
	{
		auto it = aliens.begin();
		while (it != aliens.end()) {
			if (CheckCollisionRecs(it->getRect(), Laser.getRect())) {
				Laser.active = false;     // Laser deactivates regardless
				it->TakeHit();            // Alien loses 1 HP
				if (it->IsDead()) {       // Only remove if HP <= 0
					switch (it->GetType()) {
					case 1: score += 100; break;
					case 2: score += 200; break;
					case 3: score += 300; break;
					}
					CheckForHighscore();
					NotifyUI();
					SoundManager::GetInstance()->PlaySoundEffectsAlienExplosion();
					it = aliens.erase(it); // Remove from list
				}
				else {
					++it; // Still alive, keep it
				}
			}
			else {
				++it; // No collision, move on
			}
		}


		// Check Laser collision with UFO
		if (CheckCollisionRecs(ufo.getRect(), Laser.getRect())) {
			ufo.alive = false;
			Laser.active = false;
			SoundManager::GetInstance()->PlaySoundEffectsUFOExplosion();
			score += 500;
			CheckForHighscore();
			NotifyUI();
			if (lives < 3)
			{
				lives++;
				NotifyUI();
			}
		}
	}

	//Alien Laser with spaceship and obstacle

	for (auto& Laser : alienLaser) {
		if (CheckCollisionRecs(Laser.getRect(), SpaceShip.getRect())) {
			Laser.active = false;
			//std::cout << "SpaceShip Getting Hit" << std::endl;
			SoundManager::GetInstance()->PlaySoundEffectsSpaceShipExplosion();
			lives--;
			if (lives <= 0) {
				SoundManager::GetInstance()->PlaySoundEffectsSpaceShipExplosion();
				GameOver();
			}
			NotifyUI();
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
			SoundManager::GetInstance()->PlaySoundEffectsSpaceShipExplosion();
			GameOver();
		}
	}
}

void Game::GameOver()
{
	std::cout << "Game Over" << std::endl;
	run = false;

}

void Game::NextLevel()
{
	
	SpaceShip.Reset();
	aliens.clear();
	alienLaser.clear();
	NextLevelInit();
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
	//init variable (please be on the top before spawning stuff, since its a reset)
	alienSpeedMultiplier = 0;
	alienLocationDiff = 0;
	lives = 3;
	score = 0;
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
	//Highscore = loadHighScoreFromFile();
	NotifyUI();
	
}

void Game::NextLevelInit()
{
	//I might be a damn genius for doing this stuff just for sacrificing my sleep 
	//The after toilet dump effects might be real
	NumberOfLevel = NumberOfLevel++;
	NotifyUI();
	//std::cout << (NumberOfLevel) << std::endl;
	alienSpeedMultiplier = alienSpeedMultiplier + 1;
	aliensDownPixel = aliensDownPixel + alienSpeedMultiplier;
	alienLocationDiff = alienLocationDiff + 1;
	//std::cout << (aliensDownPixel) << std::endl;
	//std::cout << (alienSpeedMultiplier) << std::endl;
	aliens = CreateAliens();
	timeLastAlienLaser = 0.0;
	ufoLastSpawnTime = 0.0;
	ufoSpawnInterval = GetRandomValue(10 + alienSpeedMultiplier, 5 + alienSpeedMultiplier);
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

