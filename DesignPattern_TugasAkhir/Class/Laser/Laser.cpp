#include "Laser.h"
#include <iostream>

Laser::Laser(Vector2 position, int speed)
{
	this -> position = position;
	this -> speed = speed;
	active = true;
}

void Laser::Draw() 
{
	if(active)
		DrawRectangle(position.x, position.y, 4, 15, { GREEN });
}

Rectangle Laser::getRect()
{
	Rectangle rect;
	rect.x = position.x;
	rect.y = position.y;
	rect.width = 4;
	rect.height = 15;
	return rect;
}

void Laser::update()
{
	position.y += speed;
	if (active) {
		if (position.y > GetScreenHeight() - 100 || position.y < 25) {
			active = false;
			//std::cout << "Laser Inactive" << std::endl;	//Print string if the Laser is inactive
		}
	}
}