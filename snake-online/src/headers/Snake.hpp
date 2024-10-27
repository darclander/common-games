#pragma once

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Grid.hpp"
#include "Observer.hpp"

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
        Snakeblock(SDL_Renderer *renderer, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight, SDL_Texture *textureSnakeHead, int degrees, SDL_Color color);
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
        SDL_Color m_color;

        SDL_Rect m_snakeblock;
        SDL_Rect m_snakeblockOverlay;
        SDL_Renderer *m_renderer;

        SDL_Texture *m_textureSnakeHead;

};

class Snake : public Observer {

    public:
        Snake(SDL_Renderer *renderer, int xPos, int yPos, Grid *grid, int snakeWidth, int snakeHeight, int snakeSize, SDL_Color color);
        Snake() {
            
        }
        ~Snake();

        void render();
        void update(double deltaTime, float limit);

        void updatePos(int xPos, int yPos);

        void grow();

        int getPosX() {
            return snakeBlocks[0].getPosX();
        }

        int getPosY() {
            return snakeBlocks[0].getPosY();
        }

        int getSize() {
            return snakeBlocks.size();
        }

        bool isHead(int xPos, int yPos) {
            return (snakeBlocks[0].getPosX() == xPos) && (snakeBlocks[0].getPosY() == yPos);
        }

        void getPositions() {
            for (auto &sb : snakeBlocks) {
                std::cout << sb.getPosX() << ", " << sb.getPosY() << std::endl; 
            }
        }

        void onEvent(const SDL_Event& event) override;

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

        SDL_Renderer *m_renderer;
        SDL_Texture *m_textureSnakeHead;
        SDL_Color m_color;

        std::vector<Snakeblock> snakeBlocks;

};

