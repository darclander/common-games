#include "Gui.hpp"

GUI::GUI(const char *title, int windowWidth, int windowHeight, bool fullscreen) {
    int flags = 0;
    this->m_windowWidth = windowWidth;
    this->m_windowHeight = windowHeight;
    this->m_windowClose = false;
    *m_camera = Camera();

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

SDL_Texture *GUI::loadTexture(std::string name, const std::string &filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
    }

    m_textureMap[name] = texture;

    return texture;  
}

SDL_Texture *GUI::getTexture(const std::string &key) {
    // Retrieve the texture associated with the key
    auto it = m_textureMap.find(key);
    if (it != m_textureMap.end()) {
        return it->second;
    }

    return nullptr;  // Texture not found
}

void GUI::unloadTexture(const std::string &key) {
    // Unload the texture associated with the key
    auto it = m_textureMap.find(key);
    if (it != m_textureMap.end()) {
        SDL_DestroyTexture(it->second);
        m_textureMap.erase(it);
    }
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
    std::cout << m_camera->getXPos() << std::endl;
    std::cout << m_camera->getYPos() << std::endl;

}

Text GUI::createText(const std::string &name, int xPos, int yPos, SDL_Color textColor, std::string font) {
    
    SDL_Surface *textSurface = TTF_RenderText_Solid(m_fonts[font], name.c_str(), textColor);

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
    textInfo.texture    = textTexture;
    textInfo.xPos       = xPos;
    textInfo.yPos       = yPos;
    textInfo.color      = textColor;
    textInfo.name       = name;
    textInfo.font       = m_fonts[font];
    textInfo.renderer   = m_renderer;

    SDL_FreeSurface(textSurface);
    return textInfo;
}

bool GUI::updateTextColor(Text &txt, SDL_Color textColor) {
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

bool GUI::updateTextPos(Text &txt, int xPos, int yPos) {
    txt.xPos = xPos;
    txt.yPos = yPos;
    return true;
}

bool GUI::updateTextValue(Text &txt, std::string newValue) {
    txt.name = newValue;
    SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, txt.name.c_str(), txt.color);
    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    // Update text dimensions.
    txt.width      = textSurface->w;
    txt.height     = textSurface->h;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
    }
    SDL_FreeSurface(textSurface);

    txt.texture = textTexture;
    return true;
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

Camera *GUI::getCamera() {
    return m_camera;
}

int GUI::getCenterX() {
    return m_windowWidth / 2;
}

int GUI::getCenterY() {
    return m_windowHeight / 2;
}

bool GUI::loadFont(std::string name, std::string path, int f_size) {
    // Check if the font is already loaded
        if (m_fonts.find(name) != m_fonts.end()) {
            // Font with the same key already exists
            return false;
        }

        // Load the font using SDL_ttf
        TTF_Font* font = TTF_OpenFont(path.c_str(), f_size);
        if (!font) {
            // Failed to load the font
            return false;
        }

        // Add the loaded font to the map
        m_fonts[name] = font;

        return true;
}

TTF_Font *GUI::getFont() {
    return m_font;
}

GUI::~GUI() {
    TTF_CloseFont(m_font);
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}