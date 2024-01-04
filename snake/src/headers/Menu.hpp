#pragma once

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <thread>

#include "Controller.hpp"

#define MENU_OPTION     0x0
#define MENU_BAR        0x1

namespace menuc {
    const SDL_Color RED     = {255, 0,   0,   255};
    const SDL_Color GREEN   = {0,   255, 0,   255};
    const SDL_Color BLUE    = {0,   0,   255, 255};
    const SDL_Color WHITE   = {255, 255, 255, 255};
}

struct Text {
    int width;
    int height;
    int xPos;
    int yPos;
    std::string name;
    SDL_Texture *texture;
};

struct MenuItem {
    Text menuText;
    int menuWidth;
    int menuHeight;
    int type;
};

class Menu : public Observer {

    public:
        Menu(SDL_Renderer *renderer, int xPos, int yPos, int width, int height, TTF_Font *font);
        ~Menu();

        void onEvent(const SDL_Event& event) override;

        template <typename T>
        int addItem(const std::string &name, int type, T &referenceValue);

        void render();
        int update(double deltaTime, bool gameRunning); 

    private:

        std::thread m_menuThread;

        int m_width;
        int m_height;
        int m_xPos;
        int m_yPos;
        int m_menuIndex;
        int m_limit;
        bool m_running;

        SDL_Event m_event;

        void updateMenu();
        Text createText(const std::string &name, int xPos, int yPos, SDL_Color textColor);
        bool updateText(Text &t, SDL_Color textColor);
        std::vector<MenuItem> m_items;

        TTF_Font *m_font;
        SDL_Renderer *m_renderer;

};