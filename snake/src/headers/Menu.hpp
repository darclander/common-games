#pragma once

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

class Menu {

    public:
        Menu(SDL_Renderer *renderer, TTF_Font *font);
        ~Menu();

        int addItem(const std::string &value);

        void render();

    private:
        std::vector<std::string> m_items;
        TTF_Font *m_font;
        SDL_Renderer *m_renderer;

};