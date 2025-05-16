#include "Game.h"
#include <iostream>

Game::Game()
{
	obstacles = CreateObstacles();
}

Game::~Game()
{
	
}

void Game::update()
{
	//To make the laser move update position.y
	for (auto& laser : SpaceShip.lasers) {
		laser.update();
	}
	
	DeleteInactiveLaser();
	//std::cout << "Vector size : " << SpaceShip.lasers.size() << std::endl;
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

}



void Game::HandleInput()
{
	if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
	{
		SpaceShip.MoveLeft();
	}
	else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
		SpaceShip.MoveRight();
	}
	else if (IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		SpaceShip.ShootLaser();
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
}

std::vector<Obstacle> Game::CreateObstacles()
{
	int obstacleWidth = Obstacle::grid[0].size() * 3;
	float gap = (GetScreenWidth() - (4 * obstacleWidth)) / 5;

	for (int i = 0; i < 4; i++) {
		float offsetX = (i + 1) * gap + i * obstacleWidth;
		obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight()-100)} ) );
	}
	return obstacles;
}

