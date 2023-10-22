#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Grid.hpp"

class GUI {

    public:
        GUI(const char *title, int windowWidth, int windowHeight, bool fullscreen);
        ~GUI();

        void render();
        void update();
        void clearRenderer();

        bool getWindowClose();


    private:
        int m_windowWidth; 
        int m_windowHeight;
        
        bool m_windowClose;

        SDL_Renderer *m_renderer;
        SDL_Window *m_window;
        SDL_Event m_event;


};