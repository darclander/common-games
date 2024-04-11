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

    int m_volumeMax = 128;
    int m_volumeMin = 0;
    int m_step; 

    Mix_Chunk* loadChunk(const char* filePath);
    void onEvent(const SDL_Event& event) override;
};