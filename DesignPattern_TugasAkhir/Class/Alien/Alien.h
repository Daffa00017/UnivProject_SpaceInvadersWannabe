#pragma once
#include <raylib.h>

class Alien {
public:
    Alien(int type, Vector2 position, int health);  // health to constructor
    void Draw();
    void Update(int direction);
    int GetType() const;
    void TakeHit();
    bool IsDead() const;
    Rectangle getRect() const;
    static void UnloadImages();

    static Texture2D alienImages[3];
    Vector2 position;

private:
    int type;
    int hp; // set through factory
};
