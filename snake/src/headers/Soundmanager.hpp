#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unordered_map>
#include <string>

#include "Controller.hpp"


class SoundManager : public Observer {
public:
    SoundManager(int &volume, int &playSound);
    ~SoundManager();


    bool loadSound(const char* filePath, const char* soundKey);
    void playSound(const char* soundKey, int loops = 0);
    void stopSound(const char* soundKey);
    void setVolume(int volume);
    void setSoundVolume(const char* soundKey, int volume); // New function for volume control

    void increaseVolume();
    void decreaseVolume();

    void setVolumeAll();
    void stopSoundAll();
    void playSoundAll();

    void toggleSound();

private:
    std::unordered_map<std::string, Mix_Chunk*> m_soundMap;

    int *m_volume;
    int *m_playSound;
    int m_loops;

    bool m_soundOn = true;

    int m_volumeMax = 100;
    int m_volumeMin = 0;
    int m_step; 

    Mix_Chunk* loadChunk(const char* filePath);
    void onEvent(const SDL_Event& event) override;
};

// https://www.beepbox.co/#9n31s0k0l00e03t2ma7g0fj07r1i0o432T8v1u08f0q00d03x660W7E3070807T1v4ueaf12taq0y10r53d18A8F8B8Q126bPda58E4b862863g78T1v0u44f0qwx10l511d03A1F0B7Q005dPd444E5b661862763677T2v1u15f10w4qw02d03w0E0b4h400000000h4g000000014h000000004h400000000p1AFEZphtj8QFH-c78Y1Nizd0kTjWNdHYOg0000