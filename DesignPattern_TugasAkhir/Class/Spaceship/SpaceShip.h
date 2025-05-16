#pragma once
#include <raylib.h>
#include <vector>
#include "../Laser/Laser.h"

class SpaceShip {
public:
	SpaceShip();
	~SpaceShip();
	void Draw();
	void MoveLeft();
	void MoveRight();
	void ShootLaser();
	std::vector<Laser> lasers;

private:
	Texture2D Image;
	Vector2 position;
	int MovementRight = 30;
	int MovementLeft = 30;
	double lastFireTime;

}; 
