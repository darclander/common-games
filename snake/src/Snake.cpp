#include "Snake.hpp"

Snake::Snake(SDL_Renderer *renderer, int snakeWidth, int snakeHeight, int snakeSize) {

    this->m_renderer = renderer;
    this->m_snakeWidth = snakeWidth;
    this->m_snakeHeight = snakeHeight;
    this->m_snakeSize = snakeSize;

    m_snakeDirection = DIR_RIGHT;

    for(int i = 0; i < snakeSize; i++) {
        snakeBlocks.push_back(Snakeblock(m_renderer, 10 + i*m_snakeWidth, 10, m_snakeWidth-2, m_snakeHeight-2));
    }

}

Snake::~Snake() {

}

void Snake::render() {

    for(auto &sb : snakeBlocks) {
        sb.render();
    }

}

bool operator!=(const direction& lhs, const direction& rhs) {
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}

void Snake::update(double deltaTime, float limit) {

    m_limit += deltaTime;

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    
    if(key_state[SDL_SCANCODE_S] || key_state[SDL_SCANCODE_DOWN]) {
        if(m_snakeDirection != DIR_UP) m_snakeDirection = DIR_DOWN;        
    }

    if(key_state[SDL_SCANCODE_W] || key_state[SDL_SCANCODE_UP]) {
        if(m_snakeDirection != DIR_UP) m_snakeDirection = DIR_UP;
    }

    if(key_state[SDL_SCANCODE_D] || key_state[SDL_SCANCODE_RIGHT]) {
        if(m_snakeDirection != DIR_LEFT) m_snakeDirection = DIR_RIGHT;
    }

    if(key_state[SDL_SCANCODE_A] || key_state[SDL_SCANCODE_LEFT]) {
        if(m_snakeDirection != DIR_RIGHT) m_snakeDirection = DIR_LEFT;
    }

    if(m_limit < limit) return;
    m_limit = 0;

    snakeBlocks.pop_back();
    int newPosX = (snakeBlocks[0].getPosX() + m_snakeDirection.x * m_snakeWidth);
    int newPosY = (snakeBlocks[0].getPosY() + m_snakeDirection.y * m_snakeHeight);
    Snakeblock newSnakeBlock = Snakeblock(m_renderer, newPosX, newPosY, m_snakeWidth - 2, m_snakeHeight - 2);
    snakeBlocks.insert(snakeBlocks.begin(), newSnakeBlock);

}


Snakeblock::Snakeblock(SDL_Renderer *renderer, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight) {

    this->m_snakeBlockXpos = snakeBlockXpos;
    this->m_snakeBlockYpos = snakeBlockYpos;
    this->m_snakeBlockWidth = snakeBlockWidth;
    this->m_snakeBlockheight = snakeBlockHeight;
    this->m_renderer = renderer;

    snakeblock.w = m_snakeBlockWidth;
    snakeblock.h = m_snakeBlockheight;
    snakeblock.x = m_snakeBlockXpos;
    snakeblock.y = m_snakeBlockYpos;
}

void Snakeblock::render() {
    SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
    SDL_RenderDrawRect(m_renderer, &snakeblock);
    SDL_RenderFillRect(m_renderer, &snakeblock);
}

int Snakeblock::getPosX() {
    return m_snakeBlockXpos;
}

int Snakeblock::getPosY() {
    return m_snakeBlockYpos;
}

Snakeblock::~Snakeblock() {

}