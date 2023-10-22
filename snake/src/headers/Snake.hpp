#pragma once

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct direction {
    int x;
    int y;
};

#define DIR_UP      direction{ 0, -1}
#define DIR_DOWN    direction{ 0,  1}
#define DIR_RIGHT   direction{ 1,  0}
#define DIR_LEFT    direction{-1,  0}

class Snakeblock {

    public: 
        Snakeblock(SDL_Renderer *renderer, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight);
        ~Snakeblock();

        void render();

        int getPosX();
        int getPosY();

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
        void update(double deltaTime, float limit);

    private:
        SDL_Renderer *m_renderer;

        int m_snakeSize;
        int m_snakeWidth;
        int m_snakeHeight;

        double m_limit;
        
        direction m_snakeDirection;

        std::vector<Snakeblock> snakeBlocks;

};

