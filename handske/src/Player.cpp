#include "Player.hpp"


Player::Player(GUI *gui, Grid *grid, std::string textureName) {

    m_gui           = gui;
    m_grid          = grid;
    m_playerTexture = m_gui->getTexture(textureName);
    m_renderer      = m_gui->getRenderer();
    m_camera        = m_gui->getCamera();
    m_xPosWorld     = 0;
    m_yPosWorld     = 0;
    m_xPos          = m_xPosWorld;
    m_yPos          = m_xPosWorld;
    m_newPosX       = m_xPosWorld;
    m_newPosY       = m_yPosWorld;
    m_width         = 50;
    m_height        = 50;

    m_playerDestinationRect = {m_xPosWorld, m_yPosWorld, // x, y
                                m_width, m_height}; // width, height

    m_speed         = 3;
}

void Player::onEvent(const SDL_Event& event) {
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if(key_state[SDL_SCANCODE_S] || key_state[SDL_SCANCODE_DOWN]) {
        m_newPosY = m_yPosWorld + m_speed;
    }

    if(key_state[SDL_SCANCODE_W] || key_state[SDL_SCANCODE_UP]) {
        m_newPosY = m_yPosWorld - m_speed;
    }

    if(key_state[SDL_SCANCODE_D] || key_state[SDL_SCANCODE_RIGHT]) {
        m_newPosX = m_xPosWorld + m_speed;
    }

    if(key_state[SDL_SCANCODE_A] || key_state[SDL_SCANCODE_LEFT]) {
        m_newPosX = m_xPosWorld - m_speed;
    }
}


void Player::update(double deltaTime, float limit) {

    m_limit += deltaTime; 

    if(m_limit < limit) return;
    m_limit = 0;
    m_playerDestinationRect.x = m_xPosWorld - m_camera->getXPos();
    m_playerDestinationRect.y = m_yPosWorld - m_camera->getYPos();
    Gridpoint *newGpX = m_grid->getPoint(m_newPosX, m_yPosWorld);
    Gridpoint *newGpY = m_grid->getPoint(m_xPosWorld, m_newPosY);
    if(newGpX != nullptr && newGpX->isMovable()) {
        m_xPosWorld = m_newPosX;
    }

    if(newGpY != nullptr && newGpY->isMovable()) {
        m_yPosWorld = m_newPosY;
    }

    int cameraX = m_xPosWorld + (m_width / 2) - m_gui->getCenterX(); /* Centralize around X axis. */
    int cameraY = m_yPosWorld + (m_height / 2) - m_gui->getCenterY(); /* Centralize around Y axis. */
    m_camera->updatePos(cameraX, cameraY);

}

void Player::render() {
    SDL_RenderCopy(m_renderer, m_playerTexture, NULL, &m_playerDestinationRect);
}