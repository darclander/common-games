#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Gui.hpp"

class Score {
    public:
        Score(GUI *ui, int width, int height);
        ~Score();

        void render();
        void move(int xPos, int yPos);

    private:

        int m_xPos;
        int m_yPos;
        int m_width;
        int m_height;

        SDL_Texture *m_textureScore;
        SDL_Renderer *m_renderer;
        GUI *m_gui;

};