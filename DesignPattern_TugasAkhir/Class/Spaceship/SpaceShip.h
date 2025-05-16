#pragma once
#include <raylib.h>

class SpaceShip {
public:
	SpaceShip();
	~SpaceShip();
	void Draw();
	void MoveLeft();
	void MoveRight();
	void ShootLaser();

private:
	Texture2D Image;
	Vector2 position;

}; 
