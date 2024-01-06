#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unordered_map>
#include <string>


class SoundManager {
public:
    SoundManager();
    ~SoundManager();


    bool loadSound(const char* filePath, const char* soundKey);
    void playSound(const char* soundKey, int loops = 0);
    void stopSound(const char* soundKey);
    void setVolume(const char* soundKey, int volume); // New function for volume control

private:
    std::unordered_map<std::string, Mix_Chunk*> soundMap;

    Mix_Chunk* loadChunk(const char* filePath);
};

