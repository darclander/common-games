#include "Snake.hpp"

Snake::Snake(SDL_Renderer *renderer, Grid *grid, int snakeWidth, int snakeHeight, int snakeSize) {

    this->m_renderer = renderer;
    // this->m_snakeWidth = snakeWidth;
    // this->m_snakeHeight = snakeHeight;
    this->m_snakeSize = snakeSize;
    this->m_grid = grid;

    m_snakeDirection = DIR_RIGHT;
    m_newSnakeDirection = m_snakeDirection;

    m_snakeWidth = m_grid->getGridPointWidth(); // Retrieve from grid
    m_snakeHeight = m_grid->getGridPointHeight(); // Retrieve from grid 

    SDL_Surface *snakeHead = IMG_Load("./textures/head.png");

    m_textureSnakeHead = SDL_CreateTextureFromSurface(m_renderer, snakeHead);

    SDL_FreeSurface(snakeHead);

    for(int i = 0; i < snakeSize; i++) {
        snakeBlocks.push_back(Snakeblock(m_renderer, i*m_snakeWidth, 1, m_snakeWidth-2, m_snakeHeight-2, m_textureSnakeHead, m_degrees));
    }

}

Snake::~Snake() {
    SDL_DestroyTexture(m_textureSnakeHead);
}

void Snake::render() {

    for(int i = 0; i < snakeBlocks.size(); i++) {
        if(i == 0) {
            snakeBlocks[i].renderHead();
        } else {
            snakeBlocks[i].render();
        }
    }

}

bool operator!=(const direction& lhs, const direction& rhs) {
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}

void Snake::update(double deltaTime, float limit) {

    m_limit += deltaTime;   

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if(key_state[SDL_SCANCODE_S] || key_state[SDL_SCANCODE_DOWN]) {
        if(m_snakeDirection != DIR_UP) {
            m_newSnakeDirection = DIR_DOWN;
            m_newDegrees = 90;
        }
    }

    if(key_state[SDL_SCANCODE_W] || key_state[SDL_SCANCODE_UP]) {
        if(m_snakeDirection != DIR_DOWN) {
            m_newSnakeDirection = DIR_UP;
            m_newDegrees = -90;
        }
    }

    if(key_state[SDL_SCANCODE_D] || key_state[SDL_SCANCODE_RIGHT]) {
        if(m_snakeDirection != DIR_LEFT) {
            m_newSnakeDirection = DIR_RIGHT;
            m_newDegrees = 0;
        }
    }

    if(key_state[SDL_SCANCODE_A] || key_state[SDL_SCANCODE_LEFT]) {
        if(m_snakeDirection != DIR_RIGHT) {
            m_newSnakeDirection = DIR_LEFT;
            m_newDegrees = 180;
        }
    }

    if(m_limit < limit) return;
    m_limit = 0;
    m_snakeDirection = m_newSnakeDirection;
    m_degrees = m_newDegrees;

    int newPosX = (snakeBlocks[0].getPosX() + m_snakeDirection.x * m_snakeWidth);
    int newPosY = (snakeBlocks[0].getPosY() + m_snakeDirection.y * m_snakeHeight);    
    Gridpoint *newPoint = m_grid->getPoint(newPosX + m_snakeWidth / 2, newPosY + m_snakeHeight / 2);

    int oldPosX = snakeBlocks.back().getPosX();
    int oldPosY = snakeBlocks.back().getPosY();
    Gridpoint *oldPoint = m_grid->getPoint(oldPosX + m_snakeWidth / 2, oldPosY + m_snakeHeight / 2);

    if(oldPoint != nullptr) oldPoint->setEmpty();
    if(newPoint != nullptr) {
        if(!newPoint->isEmpty()) {
            std::cout << "GAME OVER!" << std::endl;
        }

        if(newPoint->hasScore()) {
            snakeBlocks.push_back(Snakeblock(m_renderer, (snakeBlocks.size()-1)*m_snakeWidth, 1, m_snakeWidth-2, m_snakeHeight-2, m_textureSnakeHead, m_degrees));
            newPoint->removeScore();
        }
        newPoint->setNotEmpty();

        snakeBlocks.pop_back();
        newPosX = newPoint->getGridPointX() + 2;
        newPosY = newPoint->getGridPointY() + 2;
        Snakeblock newSnakeBlock = Snakeblock(m_renderer, newPosX, newPosY, m_snakeWidth - 2, m_snakeHeight - 2, m_textureSnakeHead, m_degrees);
        snakeBlocks.insert(snakeBlocks.begin(), newSnakeBlock);
    } else {
        return;
    }

    

}


Snakeblock::Snakeblock(SDL_Renderer *renderer, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight, SDL_Texture *textureSnakeHead, int degrees) {

    this->m_snakeBlockXpos = snakeBlockXpos;
    this->m_snakeBlockYpos = snakeBlockYpos;
    this->m_snakeBlockWidth = snakeBlockWidth;
    this->m_snakeBlockheight = snakeBlockHeight;
    this->m_renderer = renderer;
    this->m_textureSnakeHead = textureSnakeHead;
    this->m_degrees = degrees;

    m_snakeblock.w = m_snakeBlockWidth;
    m_snakeblock.h = m_snakeBlockheight;
    m_snakeblock.x = m_snakeBlockXpos;
    m_snakeblock.y = m_snakeBlockYpos;

    m_snakeblockOverlay.w = m_snakeBlockWidth - 4;
    m_snakeblockOverlay.h = m_snakeBlockWidth - 4;
    m_snakeblockOverlay.x = m_snakeBlockXpos + 2;
    m_snakeblockOverlay.y = m_snakeBlockYpos + 2;
}

void Snakeblock::render() {
    SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
    SDL_RenderDrawRect(m_renderer, &m_snakeblock);
    SDL_RenderFillRect(m_renderer, &m_snakeblock);

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(m_renderer, &m_snakeblockOverlay);
    SDL_RenderFillRect(m_renderer, &m_snakeblockOverlay);
}

void Snakeblock::renderHead() {
    this->render();
    // SDL_Rect tmp;
    // tmp.w = m_snakeBlockWidth * 2;
    // tmp.h = m_snakeBlockheight * 2;
    // tmp.x = m_snakeBlockXpos - tmp.w / 4;
    // tmp.y = m_snakeBlockYpos - tmp.h / 4;
    // SDL_RenderCopyEx(m_renderer, m_textureSnakeHead, NULL, &tmp, m_degrees, NULL, SDL_FLIP_NONE);
}

int Snakeblock::getPosX() {
    return m_snakeBlockXpos;
}

int Snakeblock::getPosY() {
    return m_snakeBlockYpos;
}

Snakeblock::~Snakeblock() {

}