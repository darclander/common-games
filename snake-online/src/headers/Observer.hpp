#pragma once

#include <iostream>

#include <SDL2/SDL.h>


class Observer {
    public:
        virtual void onEvent(const SDL_Event& event) {};
        virtual void onServerMessage(const std::string &message) {};
};
