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

struct Text {
    int width;
    int height;
    int xPos;
    int yPos;
    std::string name;
    SDL_Texture *texture;
    SDL_Renderer *renderer;

    void updateX(int newX) {
        xPos = newX;
    }

    void updateY(int newY) {
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

        Text createText(const std::string &name, int xPos, int yPos, SDL_Color textColor);
        bool updateText(Text &t, SDL_Color textColor);

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
        // std::vector<std::unique_ptr<Menu>> menus;
};