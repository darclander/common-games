#pragma once

#include <iostream>
#include <vector>
#include <fstream>

#include <SDL2/SDL.h>

#include "Gui.hpp"

namespace g_type {
    const int wall    = 0x0;
    const int tile    = 0x1;
}


class Gridpoint {

    public: 
        Gridpoint(GUI *gui, int xPos, int yPos, int width, int height, int type);
        ~Gridpoint();

        void render();

        bool contains(int x, int y);

        bool isEmpty();
        bool isMovable();

        void setNotEmpty();

        void setEmpty();

        void setScore();
        void removeScore();

        bool hasScore();

        int getGridPointX();
        int getGridPointY();

        int getGridPointWidth();
        int getGridPointHeight();

    private:
        GUI             *m_gui;
        SDL_Renderer    *m_renderer;
        SDL_Texture     *m_texture;

        int m_type;
        int m_gridPointX;
        int m_gridPointY;
        int m_gridWidth;
        int m_gridHeight;
        bool m_gridPointIsEmpty = true;
        bool m_gridPointHasScore = false;
        bool m_isMovable = true;
    
};

class Grid {

    public:
        Grid(GUI *gui, int width, int height, int granularityX, int granularityY);
        ~Grid();

        void render();

        int getGridPointWidth();
        int getGridPointHeight();

        bool loadMap(const std::string &filePath);

        Gridpoint *getPoint(int x, int y);
        bool isPointEmpty(Gridpoint point);

    private: 
        GUI             *m_gui;
        SDL_Renderer    *m_renderer;

        int m_gridWidth;
        int m_gridHeight;
        int m_granularity;
        int m_granularityX;
        int m_granularityY;
        int m_gridPointWidth;
        int m_gridPointHeight;

        std::vector<Gridpoint> m_gridpoints;
};