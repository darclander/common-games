#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Score {
    public:
        Score(SDL_Renderer *renderer, int width, int height);
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

};