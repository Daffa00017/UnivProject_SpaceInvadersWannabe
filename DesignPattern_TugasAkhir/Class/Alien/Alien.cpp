#include "Alien.h"

Texture2D Alien::alienImages[3] = {};

Alien::Alien(int type, Vector2 position, int health) : type(type), position(position)
{
    if (alienImages[type - 1].id == 0) {
        switch (type) {
        case 1:
            alienImages[0] = LoadTexture("Graphics/alien_1.png");
            break;
        case 2:
            alienImages[1] = LoadTexture("Graphics/alien_2.png");
            break;
        case 3:
            alienImages[2] = LoadTexture("Graphics/alien_3.png");
            break;
        }
    }

    hp = health; // passed in via factory
}


void Alien::Draw()
{
    DrawTextureV(alienImages[type - 1], position, WHITE);
}

void Alien::Update(int direction)
{
    position.x += direction;
}

int Alien::GetType() const
{
    return type;
}

void Alien::TakeHit()
{
    hp--;
}

bool Alien::IsDead() const
{
    return hp <= 0;
}

Rectangle Alien::getRect() const
{
    return { position.x, position.y, float(alienImages[type - 1].width), float(alienImages[type - 1].height) };
}

void Alien::UnloadImages()
{
    for (int i = 0; i < 3; i++) {
        if (alienImages[i].id != 0) {
            UnloadTexture(alienImages[i]);
            alienImages[i].id = 0;
        }
    }
}
