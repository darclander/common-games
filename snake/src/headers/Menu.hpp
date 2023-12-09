#pragma once

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

#define MENU_OPTION     0x0
#define MENU_BAR        0x1

struct Text {
    int width;
    int height;
    int xPos;
    int yPos;
    SDL_Texture *texture;
};

struct MenuItem {
    Text menuText;
    int menuWidth;
    int menuHeight;
    int type;
};

class Menu {

    public:
        Menu(SDL_Renderer *renderer, int xPos, int yPos, int width, int height, TTF_Font *font);
        ~Menu();


        template <typename T>
        int addItem(const std::string &name, T &referenceValue);

        void render();

    private:

        int m_width;
        int m_height;
        int m_xPos;
        int m_yPos;

        Text createText(const std::string &name, int xPos, int yPos);
        std::vector<MenuItem> m_items;

        TTF_Font *m_font;
        SDL_Renderer *m_renderer;

};