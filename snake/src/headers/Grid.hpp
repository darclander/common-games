#pragma once

#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

class Gridpoint {

    public: 
        Gridpoint(SDL_Renderer *renderer, int xPos, int yPos, int width, int height);
        ~Gridpoint();

        void render();

        bool contains(int x, int y);

        bool isEmpty();

        void setNotEmpty();

        void setEmpty();

    private:
        SDL_Renderer *m_renderer;

        int m_gridPointX;
        int m_gridPointY;
        int m_gridWidth;
        int m_gridHeight;
        bool m_gridPointIsEmpty = true;
    
};

class Grid {

    public:
        Grid(SDL_Renderer *renderer, int width, int height, int granularity);
        ~Grid();

        void render();

        Gridpoint *getPoint(int x, int y);
        bool isPointEmpty(Gridpoint point);



    private: 
        SDL_Renderer *m_renderer;

        int m_gridWidth;
        int m_gridHeight;
        int m_granularity;

        std::vector<Gridpoint> m_gridpoints;
};