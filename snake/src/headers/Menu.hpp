#pragma once

#include <iostream>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <thread>
#include <functional>

#include "Controller.hpp"

#define MENU_STATE      0x0
#define MENU_OPTION     0x1
#define MENU_BAR        0x2
#define MENU_ON_OFF     0x3


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

class MenuItem {

    public:
        MenuItem(SDL_Renderer *renderer) {
            m_renderer = renderer;
        }

        std::string getTextString() {
            return m_textString;
        }

        SDL_Color getColor() {
            return m_color;
        }

        virtual void render() {
            std::cout << "Base render" << std::endl;
        }

    protected:
        Text menuText;

        SDL_Color m_color;
        SDL_Renderer *m_renderer;

    private:
        std::string m_textString;

};

class MenuState : public MenuItem {
    public:
        MenuState(SDL_Renderer *renderer, int nextState/*std::function<void()> refFunc*/) : MenuItem(renderer) {
            // m_referenceFunction = refFunc;
            m_nextState = nextState;
        }

        void render() override {
            // SDL_Rect renderQuad = {menuText.xPos, menuText.yPos, menuText.width, menuText.height};
            // SDL_RenderCopy(m_renderer, menuText.texture, nullptr, &renderQuad);
        }

    private:
        int m_nextState;
        std::function<void()> m_referenceFunction;
};

class MenuBar : public MenuItem {
    
    public:
        MenuBar(SDL_Renderer *renderer, std::function<void()> refFuncL, std::function<void()> refFuncR) : MenuItem(renderer) {
            m_referenceFunctionLeft     = refFuncL;
            m_referenceFunctionRight    = refFuncR;
        }

        void render() override {
            // SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
            // SDL_Rect rectA = {menuXpos + (menuWidth / 10), menuText.yPos, menuWidth - (menuWidth / 5), menuText.height};
            // SDL_RenderFillRect(m_renderer, &rectA);
            // SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
            // SDL_Rect rectB = {menuXpos + (menuWidth / 10), menuText.yPos, int(5*referenceValue), menuText.height};
            // SDL_RenderFillRect(m_renderer, &rectB);
        }


    private:

        std::function<void()> m_referenceFunctionLeft;
        std::function<void()> m_referenceFunctionRight;

};

// struct MenuItem {

//     Text menuText;
//     std::string textString;
//     int &referenceValue;
//     int menuWidth;
//     int menuHeight;
//     int menuXpos;
//     int menuYpos;
//     int previousState;
//     int nextState;
//     int type;
//     bool onoff;
//     std::function<void()> refFunc;


//     MenuItem(int &refValue) : referenceValue(refValue) {}

//     void render() {
//         if(type == MENU_BAR) {
//             SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
//             SDL_Rect rectA = {menuXpos + (menuWidth / 10), menuText.yPos, menuWidth - (menuWidth / 5), menuText.height};
//             SDL_RenderFillRect(m_renderer, &rectA);
//             SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
//             SDL_Rect rectB = {menuXpos + (menuWidth / 10), menuText.yPos, int(5*referenceValue), menuText.height};
//             SDL_RenderFillRect(m_renderer, &rectB);

//         } else {
//             SDL_Rect renderQuad = {menuText.xPos, menuText.yPos, menuText.width, menuText.height};
//             SDL_RenderCopy(m_renderer, menuText.texture, nullptr, &renderQuad);
//         }

//     }
// };

class Menu : public Observer {

    public:
        Menu(Controller *controller, SDL_Renderer *renderer, int menuid, int xPos, int yPos, int width, int height, TTF_Font *font, int &state, int previousState);
        ~Menu();

        

        int addItem(const std::string name, int type, int &referenceValue);

        int addItemState(const std::string &name, int nextState);

        int addItemBar(std::string name, std::function<void()> refFuncL, std::function<void()> refFuncR);


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

        Controller *m_controller;

        int *m_state;
        int m_previousState;
        
        bool m_activeMenu;
        // bool m_running;
        bool m_updateMenu;
        

        SDL_Event m_event;

        void updateMenu();
        void onEvent(const SDL_Event& event) override;
        Text createText(const std::string &name, int xPos, int yPos, SDL_Color textColor);
        bool updateText(Text &t, SDL_Color textColor);
        bool updateTextValue(Text &t, const std::string newText, MenuItem &mi);

        std::vector<std::unique_ptr<MenuItem>> m_items;

        TTF_Font *m_font;
        SDL_Renderer *m_renderer;

};