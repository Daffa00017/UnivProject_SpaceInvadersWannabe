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
	Rectangle getRect();
	void Reset();
	std::vector<Laser> Lasers;

private:
	Texture2D Image;
	Vector2 position;
	int MovementRight = 30;
	int MovementLeft = 30;
	double lastFireTime;

}; 
