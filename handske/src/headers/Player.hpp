#pragma once

#include <iostream>
#include <string>

#include <SDL2/SDL.h>

#include "Gui.hpp"
#include "Controller.hpp"

class Player : public Observer {
    public:
        Player(GUI *gui, std::string textureName);


        void render();

        // Fix better implementation in src file
        void onEvent(const SDL_Event& event) override {

        }

    private:
        GUI *m_gui;
        
        SDL_Renderer    *m_renderer;
        SDL_Rect         m_playerDestinationRect;
        SDL_Texture     *m_playerTexture;

};