#pragma once

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <vector>
#include <memory>

#include "Menu.hpp"

class GUI {

    public:
        GUI(const char *title, int windowWidth, int windowHeight, bool fullscreen);
        ~GUI();

        void render();
        void update();

        template <typename T>
        void render(T &object) {
            object.render();
        }
        void clearRenderer();

        bool getWindowClose();

        SDL_Renderer *getRenderer();
        TTF_Font *getFont();

    private:
        int m_windowWidth; 
        int m_windowHeight;
        
        bool m_windowClose;

        SDL_Renderer *m_renderer;
        SDL_Window *m_window;
        SDL_Event m_event;

        TTF_Font *m_font;

        std::vector<std::unique_ptr<Menu>> menus;


};