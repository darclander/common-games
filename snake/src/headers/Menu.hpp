#pragma once

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <thread>

#include "Controller.hpp"

#define MENU_STATE      0x0
#define MENU_OPTION     0x1
#define MENU_BAR        0x2


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

    void updateX(int newX) {
        xPos = newX;
    }

    void updateY(int newY) {
        yPos = newY;
    }

};

struct MenuItem {
    Text menuText;
    int *referenceValue;
    int menuWidth;
    int menuHeight;
    int previousState;
    int nextState;
    int type;
};

class Menu : public Observer {

    public:
        Menu(SDL_Renderer *renderer, int menuid, int xPos, int yPos, int width, int height, TTF_Font *font, int &state, int previousState);
        ~Menu();

        void onEvent(const SDL_Event& event) override;

        template <typename T>
        int addItem(const std::string &name, int type, T &referenceValue);

        int addItemState(const std::string &name, const int &newValue);

        void render();
        int update(double deltaTime, bool gameRunning);
        int getMenuIndex();

    private:


        int m_width;
        int m_height;
        int m_xPos;
        int m_yPos;
        int m_menuIndex;
        int m_limit;
        int m_menuid;

        int *m_state;
        int m_previousState;
        
        bool m_activeMenu;
        // bool m_running;
        bool m_updateMenu;
        

        SDL_Event m_event;

        void updateMenu();
        Text createText(const std::string &name, int xPos, int yPos, SDL_Color textColor);
        bool updateText(Text &t, SDL_Color textColor);

        std::vector<MenuItem> m_items;

        TTF_Font *m_font;
        SDL_Renderer *m_renderer;

};