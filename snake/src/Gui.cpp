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

    // menus.emplace_back(std::make_unique<Menu>(m_renderer, 0, 0, 100, 100, m_font));
    // int sound;
    // menus[0]->addItem("Sound", MENU_OPTION, sound);
    // menus[0]->addItem("Test", MENU_OPTION, sound);

}

void GUI::onEvent(const SDL_Event& event) {
    if(event.type == SDL_QUIT) {
        m_windowClose = true;
    }
}

void GUI::update() {

    // while(SDL_PollEvent(&m_event)) {
    //     switch (m_event.type) {
    //         case SDL_QUIT :
    //             m_windowClose = true;
    //             break;
    //         default:
    //             break;
    //     }
    // }


    // for (auto &m : menus) {
        
    //     m->render();
        
    // }

}



void GUI::render() {
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderPresent(m_renderer);
}

void GUI::renderText(Text &txt) {
    SDL_Rect renderQuad = {txt.xPos, txt.yPos, txt.width, txt.height};
    SDL_RenderCopy(m_renderer, txt.texture, nullptr, &renderQuad);
}

void GUI::renderText(std::string &text, int xPos, int yPos) {
    SDL_Surface *textSurface =
        TTF_RenderText_Solid(m_font, text.c_str(), colors::WHITE);

    if (textSurface == nullptr) {
        // Handle error (you can replace this with your error handling code)
        SDL_Log("Failed to render text surface: %s", TTF_GetError());
        return;
    }

    // Create a texture from the surface
    SDL_Texture *textTexture =
        SDL_CreateTextureFromSurface(m_renderer, textSurface);

    if (textTexture == nullptr) {
        // Handle error (you can replace this with your error handling code)
        SDL_Log("Failed to create text texture: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    // Set the position for rendering the text
    SDL_Rect destRect = {xPos, yPos, textSurface->w, textSurface->h};

    // Render the text texture
    SDL_RenderCopy(m_renderer, textTexture, nullptr, &destRect);

    // Clean up resources
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

void GUI::renderNumber(int number, int xPos, int yPos) {

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

TTF_Font *GUI::getFont() {
    return m_font;
}

Text GUI::createText(const std::string &name, int xPos, int yPos, SDL_Color textColor) {
    
    SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, name.c_str(), textColor);

    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
    }

    Text textInfo;
    textInfo.width      = textSurface->w;
    textInfo.height     = textSurface->h;
    textInfo.color      = textColor;
    textInfo.texture    = textTexture;
    textInfo.xPos       = xPos;
    textInfo.yPos       = yPos;
    textInfo.name       = name;

    SDL_FreeSurface(textSurface);
    return textInfo;
}

bool GUI::updateText(Text &txt, SDL_Color textColor) {
    SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, txt.name.c_str(), textColor);
    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
    }
    SDL_FreeSurface(textSurface);

    txt.texture = textTexture;
    return true;
}

bool GUI::updateTextValue(Text &txt, const std::string newText) {
    std::string newValue = newText;
    SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, newValue.c_str(), txt.color);
    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
    }
    SDL_FreeSurface(textSurface);

    txt.texture = textTexture;
    return true;
}

GUI::~GUI() {
    TTF_CloseFont(m_font);
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}