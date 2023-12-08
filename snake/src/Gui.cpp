#include "Gui.hpp"

GUI::GUI(const char *title, int windowWidth, int windowHeight, bool fullscreen) {
    int flags = 0;
    this->m_windowWidth = windowWidth;
    this->m_windowHeight = windowHeight;
    this->m_windowClose = false;

    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    if (SDL_Init(SDL_INIT_AUDIO || SDL_INIT_VIDEO) == 0) {
        m_window = SDL_CreateWindow(
                                    title,
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    this->m_windowWidth,
                                    this->m_windowHeight,
                                    SDL_WINDOW_RESIZABLE
        );

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC);

        if(m_renderer) {
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        }
    }

    m_font = TTF_OpenFont("./font.ttf", 28);

    if (!m_font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
    
    // Menu startMenu = Menu(m_renderer, m_font);
    // startMenu.addItem("Hello");
    // // startMenu.addItem("Test");
    // //     std::cout << "here";
    // // menus.push_back(startMenu);
    // // menus.push_back(startMenu);

    // // menus.emplace_back(std::make_shared<Menu>(startMenu));

    menus.emplace_back(std::make_unique<Menu>(m_renderer, m_font));
    menus[0]->addItem("hello");
    menus[0]->addItem("hello2");
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

    for (auto &m : menus) {
        
        m->render();
        
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

GUI::~GUI() {
    TTF_CloseFont(m_font);
}