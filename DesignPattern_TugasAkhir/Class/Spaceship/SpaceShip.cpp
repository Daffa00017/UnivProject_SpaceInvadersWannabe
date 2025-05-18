#include "SpaceShip.h"

SpaceShip::SpaceShip()
{
	Image = LoadTexture("Graphics/spaceship.png");
	position.x = (GetScreenWidth() - Image.width)/2;
	position.y = GetScreenHeight() - Image.height -100 ;
	lastFireTime = 0.0;

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
	position.x -= MovementLeft;
	if (position.x < 25) {
		position.x = 25;
	}
}

void SpaceShip::MoveRight()
{
	position.x += MovementRight;
	if (position.x > GetScreenWidth() - Image.width - 25) {
		position.x = GetScreenWidth() - Image.width - 25;
	}
}

void SpaceShip::ShootLaser()
{
	if (GetTime() - lastFireTime >= 0.35) {
		lasers.push_back(Laser({ position.x + Image.width / 2 - 2 ,position.y }, -6));
		lastFireTime = GetTime();
	}
}

Rectangle SpaceShip::getRect()
{
	return{ position.x, position.y, float(Image.width) };
}

void SpaceShip::Reset() 
{
	position.x = (GetScreenWidth() - Image.width) / 2.0f;
	position.y = GetScreenHeight() - Image.height - 100;
	lasers.clear();
}
