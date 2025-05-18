#pragma once
#include <raylib.h>

//simply said this is another implementaion of singleton for a soundmanager class 
//since the game only need 1 soundmanager in 1 runtime so singleton work quite good
class SoundManager {
private:
    static SoundManager* instance;
    SoundManager();
    Sound AlienExplosion0;
    Sound AlienExplosion1;
    Sound AlienExplosion2;
    Sound SpaceShipExplosion0;
    Sound SpaceShipExplosion1;
    Sound LaserSound;

public:
    static SoundManager* GetInstance() {
        if (!instance)
            instance = new SoundManager(); //lazy initialization
        return instance;
    }

    void playmusic();
    void loadSounds();
    void unloadSounds();
    void PlaySoundEffectsAlienExplosion();
    void PlaySoundEffectsUFOExplosion();
    void PlaySoundEffectsSpaceShipExplosion();
    void PlaySoundEffectsLaser();
};