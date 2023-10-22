#include "Gui.hpp"

GUI::GUI(const char *title, int windowWidth, int windowHeight, bool fullscreen) {
    int flags = 0;
    this->m_windowWidth = windowWidth;
    this->m_windowHeight = windowHeight;
    this->m_windowClose = false;

    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        m_window = SDL_CreateWindow(
                                    title,
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    this->m_windowWidth,
                                    this->m_windowHeight,
                                    SDL_WINDOW_RESIZABLE
        );

        m_renderer = SDL_CreateRenderer(m_window, -1, 0);

        if(m_renderer) {
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        }
    }
}

GUI::~GUI() {

}

void GUI::update() {

    SDL_PollEvent(&m_event);
    switch (m_event.type) {
        case SDL_QUIT :
            m_windowClose = true;
            break;
        default:
            break;
    }

}


void GUI::render() {
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderPresent(m_renderer);
}


void GUI::clearRenderer() {
    SDL_RenderClear(m_renderer);
}

bool GUI::getWindowClose() {
    return !m_windowClose;
}

SDL_Renderer *GUI::getRenderer() {
    return m_renderer;
}