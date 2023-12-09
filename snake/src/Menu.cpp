#include "Menu.hpp"

Menu::Menu(SDL_Renderer *renderer, int xPos, int yPos, int width, int height, TTF_Font *font) {
    m_renderer = renderer;

    m_xPos      = xPos;
    m_yPos      = yPos;
    m_width     = width;
    m_height    = height;


    m_font = font;
    std::cout << &m_items << std::endl;


}

Text Menu::createText(const std::string &name, int xPos, int yPos) {
    SDL_Color textColor = {255, 255, 255, 255}; // White color
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

    SDL_FreeSurface(textSurface);
    return textInfo;
}

template int Menu::addItem(const std::string &name, int &reference_value);
template int Menu::addItem(const std::string &name, float &reference_value);

template <typename T>
int Menu::addItem(const std::string &name, T &reference_value) {

    Text textInfo = createText(name, m_xPos, m_yPos + m_items.size() * 50);
    MenuItem mi;
    mi.menuText = textInfo;

    m_items.push_back(mi);

    return m_items.size(); 
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
}