#pragma once
#include <raylib.h>

class Alien {
public:
    Alien(int type, Vector2 Position);
    void Draw();
    void Update(int direction);
    int GetType() const;
    void TakeHit();
    bool IsDead() const;
    Rectangle getRect() const;
    static void UnloadImages();

    static Texture2D alienImages[3];

    Vector2 position;  // make public for minimal change (or add getter if preferred)

private:
    int type;
    int hp; // add hp here
};
