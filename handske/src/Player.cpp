#include "Player.hpp"


Player::Player(GUI *gui, std::string textureName) {

    m_gui           = gui;
    m_playerTexture = m_gui->getTexture(textureName);
    m_renderer      = m_gui->getRenderer();
    m_playerDestinationRect = {0, 0,  // x, y
                                50, 50}; // width, height
}

void Player::render() {
    SDL_RenderCopy(m_renderer, m_playerTexture, NULL, &m_playerDestinationRect);
}