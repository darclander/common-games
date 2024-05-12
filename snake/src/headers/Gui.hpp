#pragma once

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <vector>
#include <memory>
#include <thread>
#include <unordered_map>

// #include "Menu.hpp"
#include "Controller.hpp"

// For controlling all the colors. TODO: move this from menu.hpp
namespace g_color {
    const SDL_Color RED     = {255, 0,   0,   255};
    const SDL_Color GREEN   = {0,   255, 0,   255};
    const SDL_Color BLUE    = {0,   0,   255, 255};
    const SDL_Color WHITE   = {255, 255, 255, 255};
}

struct Point {
    int xPos;
    int yPos;

    Point(int x, int y) {
        xPos = x;
        yPos = y;
    }
};


struct Text {
    int width;
    int height;
    int xPos;
    int yPos;
    std::string name;
    SDL_Color color;
    SDL_Texture *texture;
    SDL_Renderer *renderer;

    TTF_Font *font;

    int getX() {
        return xPos;
    }

    int getY() {
        return yPos;
    }

    void updateX(int newX) {
        xPos = newX;
    }

    void updateY(int newY) {
        yPos = newY;
    }

    void updatePos(int newX, int newY) {
        xPos = newX;
        yPos = newY;
    }

    void render() {
        SDL_Rect renderQuad = {xPos, yPos, width, height};
        SDL_RenderCopy(renderer, texture, nullptr, &renderQuad);
    }
};

class GUI : public Observer {

    public:
        GUI(const char *title, int windowWidth, int windowHeight, bool fullscreen);
        ~GUI();

        void render();
        void update();

        void onEvent(const SDL_Event& event) override;

        template <typename T>
        void render(T &object) {
            object.render();
        }

        void clearRenderer();

        bool getWindowClose();

        SDL_Texture *loadTexture(std::string name, const std::string &filePath);
        SDL_Texture *getTexture(const std::string &key);
        void unloadTexture(const std::string &key); // Should be a bool to confirm?

        SDL_Renderer *getRenderer();
        
        TTF_Font *getFont();
        
        TTF_Font *getFont(std::string name);
        bool loadFont(std::string name, std::string path, int f_size);

        Text createText(const std::string &name, int xPos, int yPos, SDL_Color textColor, std::string font);
        bool updateTextColor(Text &t, SDL_Color textColor);
        bool updateTextValue(Text &t, std::string newName);
        bool updateTextPos(Text &t, int xPos, int yPos);

    private:
        int m_windowWidth; 
        int m_windowHeight;
        
        bool m_windowClose;

        // std::thread m_guiThread;

        SDL_Renderer *m_renderer;
        SDL_Window *m_window;
        SDL_Event m_event;

        TTF_Font *m_font;

        std::unordered_map<std::string, SDL_Texture*> m_textureMap;
        std::unordered_map<std::string, TTF_Font*> m_fonts;
        // std::vector<std::unique_ptr<Menu>> menus;
};