#pragma once

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Grid.hpp"
#include "Controller.hpp"

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
        Snakeblock(SDL_Renderer *renderer, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight, SDL_Texture *textureSnakeHead, int degrees);
        ~Snakeblock();

        void render(); 
        void renderHead();

        int getPosX();
        int getPosY();

    private:
        int m_snakeBlockWidth;
        int m_snakeBlockheight;

        int m_snakeBlockXpos;
        int m_snakeBlockYpos;
        int m_degrees;

        SDL_Rect m_snakeblock;
        SDL_Rect m_snakeblockOverlay;
        SDL_Renderer *m_renderer;

        SDL_Texture *m_textureSnakeHead;

};

class Snake : public Observer {

    public:
        Snake(SDL_Renderer *renderer, int xPos, int yPos, Grid *grid, int snakeWidth, int snakeHeight, int snakeSize);
        ~Snake();

        void render();
        void update(double deltaTime, float limit);

        void onEvent(const SDL_Event& event) override;

    private:
        SDL_Renderer *m_renderer;
        SDL_Texture *m_textureSnakeHead;


        int m_snakeSize;
        int m_snakeWidth;
        int m_snakeHeight;
        int m_degrees;
        int m_newDegrees;

        double m_limit;
        
        direction m_snakeDirection;
        direction m_newSnakeDirection;

        Grid *m_grid;

        std::vector<Snakeblock> snakeBlocks;

};

