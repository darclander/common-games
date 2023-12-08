#include "Menu.hpp"


Menu::Menu(SDL_Renderer *renderer, TTF_Font *font) {
    m_renderer = renderer;

    m_font = font;
    std::cout << &m_items << std::endl;


}

int Menu::addItem(const std::string &value) {
    m_items.push_back(value);
    return m_items.size(); 
}

void Menu::render() {

    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);

    SDL_Rect rect = {375, 275, 50, 50};

    SDL_RenderFillRect(m_renderer, &rect);

    SDL_Color textColor = {255, 255, 255, 255}; // White color
    const std::string s = "tst";


    SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, m_items[1].c_str(), textColor);

    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

        // Create texture from surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    // Get text dimensions
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;


    // Set rendering space and render to screen
    SDL_Rect renderQuad = {5, 5, textWidth, textHeight};
    SDL_RenderCopy(m_renderer, textTexture, nullptr, &renderQuad);

    // Cleanup
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);


}

Menu::~Menu() {
    
}