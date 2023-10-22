#pragma once

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct direction {
    int x;
    int y;
};

#define UP      direction{ 0, -1}
#define DOWN    direction{ 0,  1}
#define RIGHT   direction{ 1,  0}
#define LEFT    direction{-1,  0}

class Snakeblock {

    public: 
        Snakeblock(SDL_Renderer *renderer, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight);
        ~Snakeblock();

        void render();

    private:
        int m_snakeBlockWidth;
        int m_snakeBlockheight;

        int m_snakeBlockXpos;
        int m_snakeBlockYpos;

        SDL_Rect snakeblock;
        SDL_Renderer *m_renderer;

};

class Snake {

    public:
        Snake(SDL_Renderer *renderer, int snakeWidth, int snakeHeight, int snakeSize);
        ~Snake();

        void render();
        void update(float deltaTime);

    private:
        SDL_Renderer *m_renderer;

        int m_snakeSize;
        int m_snakeWidth;
        int m_snakeHeight;
        direction m_snakeDirection = UP;

        std::vector<Snakeblock> snakeBlocks;

};

