#pragma once

#include <iostream>
#include <string>

#include <SDL2/SDL.h>

#include "Gui.hpp"
#include "Controller.hpp"
#include "Grid.hpp"

class Player : public Observer {
    public:
        Player(GUI *gui, Grid *grid, std::string textureName);


        void render();

        void update(double deltaTime, float limit);

        // Fix better implementation in src file
        void onEvent(const SDL_Event& event) override;

    private:
        int m_speed;

        // For rendering
        int m_xPos; 
        int m_yPos;

        int m_xPosWorld; // Real X position
        int m_yPosWorld; // Real Y position

        int m_newPosX;
        int m_newPosY;

        float m_limit;
        
        int m_width;
        int m_height;

        GUI     *m_gui;
        Camera  *m_camera;
        Grid    *m_grid;

        SDL_Renderer    *m_renderer;
        SDL_Rect         m_playerDestinationRect;
        SDL_Texture     *m_playerTexture;

};