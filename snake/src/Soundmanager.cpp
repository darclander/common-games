#include "Soundmanager.hpp"

SoundManager::SoundManager() {
    // Initialize SDL audio subsystem
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
}

SoundManager::~SoundManager() {
    for (auto& pair : soundMap) {
        Mix_FreeChunk(pair.second);
    }
    soundMap.clear();

    Mix_Quit();
    SDL_Quit();
}



bool SoundManager::loadSound(const char* filePath, const char* soundKey) {
    Mix_Chunk* chunk = loadChunk(filePath);
    if (chunk == nullptr) {
        return false;
    }

    soundMap[soundKey] = chunk;
    return true;
}

void SoundManager::playSound(const char* soundKey, int loops) {
    auto it = soundMap.find(soundKey);
    if (it != soundMap.end()) {
        Mix_PlayChannel(-1, it->second, loops);
    }
}

void SoundManager::stopSound(const char* soundKey) {
    auto it = soundMap.find(soundKey);
    if (it != soundMap.end()) {
        Mix_HaltChannel(-1);
    }
}

void SoundManager::setVolume(const char* soundKey, int volume) {
    auto it = soundMap.find(soundKey);
    if (it != soundMap.end()) {
        Mix_VolumeChunk(it->second, volume);
    }
}

Mix_Chunk* SoundManager::loadChunk(const char* filePath) {
    Mix_Chunk* chunk = Mix_LoadWAV(filePath);
    if (chunk == nullptr) {
        printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    }
    return chunk;
}
