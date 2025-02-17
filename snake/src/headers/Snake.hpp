#pragma once

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Grid.hpp"
#include "Controller.hpp"
#include "Soundmanager.hpp"

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
        Snake(SDL_Renderer *renderer, SoundManager *sm, int xPos, int yPos, Grid *grid, int snakeWidth, int snakeHeight, int snakeSize);
        ~Snake();

        void render();
        bool update(double deltaTime, float limit);
        // void update(double deltaTime, float limit);

        void onEvent(const SDL_Event& event) override;

        void reset();

        int size() {
            return snakeBlocks.size();
        }

    private:
        int m_snakeSize;
        int m_snakeWidth;
        int m_snakeHeight;
        int m_degrees;
        int m_newDegrees;

        double m_limit;
        
        direction m_snakeDirection;
        direction m_newSnakeDirection;

        Grid *m_grid;
        SoundManager *m_sm;

        SDL_Renderer *m_renderer;
        SDL_Texture *m_textureSnakeHead;

        std::vector<Snakeblock> snakeBlocks;
};

