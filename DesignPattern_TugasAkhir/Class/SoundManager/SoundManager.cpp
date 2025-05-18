#include "SoundManager.h"
#include <raylib.h>

SoundManager* SoundManager::instance = nullptr; // Define the static instance

SoundManager::SoundManager() {
    loadSounds();
}


void SoundManager::playmusic()
{
    //i have yet to find a good 8 bit music for space invaders lmao 
    //(apart of the youtube one in which, im not sure if it ok for the law)
}

void SoundManager::loadSounds()
{
    //load all used sound (albeit this is not optimize since it would make the startup much longer)
    AlienExplosion0 = LoadSound("Sounds/AlienExplosion1.ogg");
    AlienExplosion1 = LoadSound("Sounds/AlienExplosion2.ogg");
    AlienExplosion2 = LoadSound("Sounds/AlienExplosion3.ogg");
    SpaceShipExplosion0 = LoadSound("Sounds/SpaceShipExplosion1.ogg");
    SpaceShipExplosion1 = LoadSound("Sounds/SpaceShipExplosion2.ogg");
    LaserSound = LoadSound("Sounds/Laser.ogg");

}

void SoundManager::unloadSounds()
{
    //unload all used sound (quite the stone age are we?)
    UnloadSound(AlienExplosion0);
    UnloadSound(AlienExplosion1);
    UnloadSound(AlienExplosion2);
    UnloadSound(SpaceShipExplosion0);
    UnloadSound(SpaceShipExplosion1);
    UnloadSound(LaserSound);

}

void SoundManager::PlaySoundEffectsAlienExplosion()
{
    int randomIndex = GetRandomValue(0, 1);
    if (randomIndex == 0) {
        PlaySound(AlienExplosion0);
    }
    else if (randomIndex == 1) {
        PlaySound(AlienExplosion1);
    }else if (randomIndex == 2) {
        PlaySound(AlienExplosion2);
    }
}

void SoundManager::PlaySoundEffectsUFOExplosion()
{
    PlaySound(AlienExplosion2);
}

void SoundManager::PlaySoundEffectsSpaceShipExplosion()
{
    int randomIndexSpaceShip = GetRandomValue(0, 1);
    if (randomIndexSpaceShip == 0) {
        PlaySound(SpaceShipExplosion0);
    }
    else if (randomIndexSpaceShip == 1) {
        PlaySound(SpaceShipExplosion1);
    }
}

void SoundManager::PlaySoundEffectsLaser()
{
    PlaySound(LaserSound);
}
