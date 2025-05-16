#include "SpaceShip.h"

SpaceShip::SpaceShip()
{
	Image = LoadTexture("Graphics/spaceship.png");
	position.x = 100;
	position.y = 100;

}

SpaceShip::~SpaceShip()
{
	UnloadTexture(Image);
}

void SpaceShip::Draw()
{

	DrawTextureV(Image, position, WHITE);

}

void SpaceShip::MoveLeft()
{

}

void SpaceShip::MoveRight()
{

}

void SpaceShip::ShootLaser()
{

}