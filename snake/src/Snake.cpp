#include "Snake.hpp"

Snake::Snake(SDL_Renderer *renderer, int snakeWidth, int snakeHeight, int snakeSize) {

    this->m_renderer = renderer;
    this->m_snakeWidth = snakeWidth;
    this->m_snakeHeight = snakeHeight;
    this->m_snakeSize = snakeSize;

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

void Snake::update(float deltaTime) {

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

Snakeblock::~Snakeblock() {

}