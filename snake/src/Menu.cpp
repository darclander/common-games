#include "Menu.hpp"

Menu::Menu(Controller *controller, SDL_Renderer *renderer, int menuid, int xPos, int yPos, int width, int height, TTF_Font *font, int &state, int previousState) {
    m_renderer = renderer;

    m_xPos          = xPos;
    m_yPos          = yPos;
    m_width         = width;
    m_height        = height;
    m_menuid        = menuid;
    // m_running       = true;
    m_updateMenu    = false;
    m_activeMenu    = false;
    m_limit         = 0;
    m_menuIndex     = 0; // Starting at first value of a menu.

    m_state         = &state;
    m_previousState = previousState;

    m_font = font;
    // m_menuThread = std::thread(&Menu::updateMenu, this);
    // std::cout << &m_items << std::endl;

    m_controller = controller;

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

bool Menu::updateTextValue(Text &txt, const std::string newText, MenuItem &mi) {
    std::string newValue = mi.textString + newText;
    SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, newValue.c_str(), mi.color);
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



template int Menu::addItem(std::string name, int type, int &reference_value);
// template int Menu::addItem(std::string name, int type, float *reference_value);

template <typename T>
int Menu::addItem(std::string name, int type, T &reference_value) {

    MenuItem mi(reference_value);
    mi.type = type;
    mi.onoff = true;
    mi.nextState = -5;
    mi.color = menuc::WHITE;
    mi.textString = name;
    mi.menuWidth = m_width;
    mi.menuHeight = m_height;
    mi.menuXpos = m_xPos;
    mi.menuYpos = m_yPos;
    mi.m_renderer = m_renderer;


    if(type == MENU_ON_OFF) {
        name = name + "on";
    }

    Text textInfo = createText(name, m_xPos, m_yPos + m_items.size() * 50, menuc::WHITE);

    textInfo.updateX(m_xPos + (m_width - textInfo.width) / 2);
    if(m_items.size() == 0) {
        updateText(textInfo, menuc::RED);
        mi.color = menuc::RED;
    }

    mi.menuText = textInfo;
    m_items.push_back(mi);

    return m_items.size(); 
}




int Menu::addItemState(const std::string &name, const int &newValue) {

    Text textInfo = createText(name, m_xPos, m_yPos + m_items.size() * 50, menuc::WHITE);
    textInfo.updateX(m_xPos + (m_width - textInfo.width) / 2);

    if(m_items.size() == 0) {
        updateText(textInfo, menuc::RED);
    }
    int x = 3;
    MenuItem mi(x);
    mi.menuText = textInfo;
    mi.nextState = newValue;
    mi.type = MENU_STATE;
    mi.m_renderer = m_renderer;

    m_items.push_back(mi);

    return m_items.size(); 
}



void Menu::updateMenu() {
    
}

void Menu::onEvent(const SDL_Event& event) {

    if(event.type == SDL_USEREVENT) {
        if(event.user.code == m_menuid) {
            m_activeMenu = true;
        } else {
            m_activeMenu = false;
        }
    }

    if (m_activeMenu) {
        if (event.type == SDL_KEYDOWN) {
            const Uint8 *key_state = SDL_GetKeyboardState(NULL);
            if(m_items.size() > 0) {
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


            if(key_state[SDL_SCANCODE_RIGHT] && m_items[m_menuIndex].type == MENU_BAR) {
                (m_items[m_menuIndex].referenceValue) += 128 / 10;
                m_controller->broadcastEvent(5); // Sound change
            }

            if(key_state[SDL_SCANCODE_LEFT] && m_items[m_menuIndex].type == MENU_BAR) {
                (m_items[m_menuIndex].referenceValue) -= 128 / 10;
                m_controller->broadcastEvent(5);
            }
        
            // if (key_state[SDL_SCANCODE_ESCAPE]) {
            //     std::cout << "ESCAPE" << std::endl;
            //     if(m_items.size() > 0) updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
            //     m_menuIndex = -2;
            //     m_updateMenu = true;
            // }

            if (key_state[SDL_SCANCODE_ESCAPE]) {
                std::cout << "PREVIOUS:" << m_previousState << std::endl;
                *m_state = m_previousState;
            }

            if (key_state[SDL_SCANCODE_RETURN]) {
                if(m_items.size() > 0) {
                // m_updateMenu = true;
                // m_items[m_menuIndex].menuText
                    if(m_items[m_menuIndex].type == MENU_STATE) {
                        updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
                        *m_state = m_items[m_menuIndex].nextState;
                    } else if (m_items[m_menuIndex].type == MENU_ON_OFF) {
                        if (m_items[m_menuIndex].onoff) {
                            updateTextValue(m_items[m_menuIndex].menuText, "off", m_items[m_menuIndex]);
                            m_items[m_menuIndex].onoff = false;
                            m_items[m_menuIndex].referenceValue = 0;
                        } else {
                            updateTextValue(m_items[m_menuIndex].menuText, "on", m_items[m_menuIndex]);
                            m_items[m_menuIndex].onoff = true;
                            m_items[m_menuIndex].referenceValue = 1;
                        }
                        m_controller->broadcastEvent(5);
                    }
                }
            }
        }
    }

}


int Menu::update(double deltaTime, bool gameRunning) {
    m_limit += deltaTime;  
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    // m_running = gameRunning;


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

int Menu::getMenuIndex() {
    if(m_updateMenu) {
        m_updateMenu = false;
        int newIndex = m_menuIndex + 1; 
        return newIndex;
    } else {
        return -2;
    }
}

void Menu::render() {

    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);

    SDL_Rect rectL = {m_xPos, m_yPos, 1, m_height};
    SDL_Rect rectR = {m_xPos + m_width, m_yPos, 1, m_height};

    SDL_RenderFillRect(m_renderer, &rectL);
    SDL_RenderFillRect(m_renderer, &rectR);

    for (auto &m : m_items) {
        // SDL_Rect renderQuad = {m.menuText.xPos, m.menuText.yPos, m.menuText.width, m.menuText.height};
        // SDL_RenderCopy(m_renderer, m.menuText.texture, nullptr, &renderQuad);
        m.render();
    }

}

Menu::~Menu() {
    for (auto &m : m_items) {
        SDL_DestroyTexture(m.menuText.texture);
    }
}