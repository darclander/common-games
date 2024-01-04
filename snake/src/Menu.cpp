#include "Menu.hpp"

Menu::Menu(SDL_Renderer *renderer, int xPos, int yPos, int width, int height, TTF_Font *font) {
    m_renderer = renderer;

    m_xPos      = xPos;
    m_yPos      = yPos;
    m_width     = width;
    m_height    = height;
    m_running   = true;
    m_limit     = 0;
    m_menuIndex = 0; // Starting at first value of a menu.

    m_font = font;
    // m_menuThread = std::thread(&Menu::updateMenu, this);
    std::cout << &m_items << std::endl;
}

Text Menu::createText(const std::string &name, int xPos, int yPos, SDL_Color textColor) {
    
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
    textInfo.texture    = textTexture;
    textInfo.xPos       = xPos;
    textInfo.yPos       = yPos;
    textInfo.name       = name;

    SDL_FreeSurface(textSurface);
    return textInfo;
}

bool Menu::updateText(Text &txt, SDL_Color textColor) {
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

template int Menu::addItem(const std::string &name, int type, int   &reference_value);
template int Menu::addItem(const std::string &name, int type, float &reference_value);

template <typename T>
int Menu::addItem(const std::string &name, int type, T &reference_value) {

    Text textInfo = createText(name, m_xPos, m_yPos + m_items.size() * 50, menuc::WHITE);
    if(m_items.size() == 0) {
        updateText(textInfo, menuc::RED);
    }
    MenuItem mi;
    mi.menuText = textInfo;

    m_items.push_back(mi);

    return m_items.size(); 
}

void Menu::updateMenu() {
    
}

void Menu::onEvent(const SDL_Event& event) {
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    if (event.type == SDL_KEYDOWN) {
        if(key_state[SDL_SCANCODE_DOWN]) {
            updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
            if(m_menuIndex < m_items.size()-1) m_menuIndex++;
            updateText(m_items[m_menuIndex].menuText, menuc::RED);
        } else if (key_state[SDL_SCANCODE_UP]) {
            updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
            if(m_menuIndex > 0) m_menuIndex--;
            updateText(m_items[m_menuIndex].menuText, menuc::RED);
        }
    }
}


int Menu::update(double deltaTime, bool gameRunning) {
    m_limit += deltaTime;  
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    m_running = gameRunning;


    // SDL_Event event;
    // SDL_PollEvent( &event );
    //     if (event.type == SDL_KEYDOWN) {
    //         std::cout << "KEYDOWN";
    //         if(key_state[SDL_SCANCODE_DOWN]) {
    //             updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
    //             if(m_menuIndex < m_items.size()-1) m_menuIndex++;
    //             updateText(m_items[m_menuIndex].menuText, menuc::RED);
    //         } else if (key_state[SDL_SCANCODE_UP]) {
    //             updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
    //             if(m_menuIndex > 0) m_menuIndex--;
    //             updateText(m_items[m_menuIndex].menuText, menuc::RED);
    //         }
    //     }
    
    // if(m_limit > 100.f) {
    //     m_limit = 0;
    //     if(key_state[SDL_SCANCODE_DOWN]) {
    //         updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
    //         if(m_menuIndex < m_items.size()-1) m_menuIndex++;
    //         updateText(m_items[m_menuIndex].menuText, menuc::RED);
    //     } else if (key_state[SDL_SCANCODE_UP]) {
    //         updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
    //         if(m_menuIndex > 0) m_menuIndex--;
    //         updateText(m_items[m_menuIndex].menuText, menuc::RED);
    //     }
    // }

    if(key_state[SDL_SCANCODE_RETURN]) {
        return m_menuIndex + 1;
    }

    return 0;
}

void Menu::render() {

    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);

    SDL_Rect rect = {375, 275, 50, 50};

    SDL_RenderFillRect(m_renderer, &rect);


    for (auto &m : m_items) {
        SDL_Rect renderQuad = {m.menuText.xPos, m.menuText.yPos, m.menuText.width, m.menuText.height};
        SDL_RenderCopy(m_renderer, m.menuText.texture, nullptr, &renderQuad);
    }

}

Menu::~Menu() {
    for (auto &m : m_items) {
        SDL_DestroyTexture(m.menuText.texture);
    }
    // m_menuThread.join();
}