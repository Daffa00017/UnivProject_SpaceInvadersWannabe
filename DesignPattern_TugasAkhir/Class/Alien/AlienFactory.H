#pragma once
#include "Alien.h"

class AlienFactory {
public:
    static Alien* CreateAlien(int type, Vector2 position) {
        Alien* newAlien = nullptr;

        switch (type) {
        case 1:
            newAlien = new Alien(type, position, 1); // Health 1 for type 1
            break;
        case 2:
            newAlien = new Alien(type, position, 2); // Health 2 for type 2
            break;
        case 3:
            newAlien = new Alien(type, position, 3); // Health 3 for type 3
            break;
        }
        return newAlien;
    }
};
