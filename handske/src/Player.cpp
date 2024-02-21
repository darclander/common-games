#include "Player.hpp"


Player::Player(GUI *gui, Grid *grid, std::string textureName) {

    m_gui           = gui;
    m_grid          = grid;
    m_playerTexture = m_gui->getTexture(textureName);
    m_renderer      = m_gui->getRenderer();
    m_playerDestinationRect = {0, 0,  // x, y
                                50, 50}; // width, height
    m_xPos          = 0;
    m_yPos          = 0;
    m_newPosX       = m_xPos;
    m_newPosY       = m_yPos;
}

void Player::onEvent(const SDL_Event& event) {
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if(key_state[SDL_SCANCODE_S] || key_state[SDL_SCANCODE_DOWN]) {
        m_newPosY = m_yPos + 1;
    }

    if(key_state[SDL_SCANCODE_W] || key_state[SDL_SCANCODE_UP]) {
        m_newPosY = m_yPos - 1;
    }

    if(key_state[SDL_SCANCODE_D] || key_state[SDL_SCANCODE_RIGHT]) {
        m_newPosX = m_xPos + 1;
    }

    if(key_state[SDL_SCANCODE_A] || key_state[SDL_SCANCODE_LEFT]) {
        m_newPosX = m_xPos - 1;
    }
}


void Player::update(double deltaTime, float limit) {

    m_limit += deltaTime; 

    if(m_limit < limit) return;
    m_limit = 0;
    m_playerDestinationRect.x = m_xPos;
    m_playerDestinationRect.y = m_yPos;

    m_xPos = m_newPosX;
    m_yPos = m_newPosY;

}

void Player::render() {
    SDL_RenderCopy(m_renderer, m_playerTexture, NULL, &m_playerDestinationRect);
}