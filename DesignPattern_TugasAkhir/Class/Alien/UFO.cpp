#include "UFO.h"

Ufo::Ufo()
{
	image = LoadTexture("Graphics/mystery.png");
	alive = false;
}

Ufo::~Ufo()
{
	UnloadTexture(image);
}

void Ufo::spawn()
{
	position.y = 90;
	int side = GetRandomValue(0, 1);

	if (side == 0)
	{
		position.x = 0;
		speed = 3;
	}
	else {
		position.x = GetScreenWidth() - image.width;
		speed = -3;
	}
	alive = true;
}

void Ufo::update() 
{
	if (alive) {
		position.x += speed;
		if (position.x > GetScreenWidth() - image.width || position.x < 0) {
			alive = false;
		}
	}
}

void Ufo::draw()
{
	if (alive) {
		DrawTextureV(image, position, WHITE);
	}
}


