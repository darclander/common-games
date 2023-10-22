#include "Grid.hpp"

Grid::Grid(SDL_Renderer *renderer, int width, int height, int granularity) {
    this->m_gridWidth = width;
    this->m_gridHeight = height;
    this->m_granularity = granularity;
    this->m_renderer = renderer;

    int rows = width / granularity;
    int cols = height / 15;

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            m_gridpoints.push_back(Gridpoint(m_renderer, i * rows, j * cols, rows, cols));
        }
    }    
}

void Grid::render() {
    for(auto &gp : m_gridpoints) {
        gp.render();
    }
}

Gridpoint *Grid::getPoint(int x, int y) {
    for (auto &gp : m_gridpoints) {
        if(gp.contains(x, y)) return &gp;
    }
    return nullptr;
}

Grid::~Grid() {}


Gridpoint::Gridpoint(SDL_Renderer *renderer, int xPos, int yPos, int width, int height) {
    this->m_renderer = renderer;
    this->m_gridPointX = xPos;
    this->m_gridPointY = yPos;
    this->m_gridWidth = width;
    this->m_gridHeight = height;
}

void Gridpoint::render() {
    SDL_Rect gpL, gpR, gpB, gpT;
    gpL.x = m_gridPointX;
    gpL.y = m_gridPointY;
    gpL.h = m_gridHeight;
    gpL.w = 1;

    gpR.x = m_gridPointX;
    gpR.y = m_gridPointY;
    gpR.h = 1;
    gpR.w = m_gridWidth;

    if(m_gridPointIsEmpty) {
        SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
    } else {
        SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
    }
    

    SDL_RenderDrawRect(m_renderer, &gpL);
    SDL_RenderFillRect(m_renderer, &gpL);
    
    SDL_RenderDrawRect(m_renderer, &gpR);
    SDL_RenderFillRect(m_renderer, &gpR);    
}

bool Gridpoint::contains(int x, int y) {
    // std::cout << "X: " << x << std::endl;
    // std::cout << "m_gridPointX" << m_gridPointX << std::endl;
    bool withinX = (x > m_gridPointX && x < m_gridPointX + m_gridWidth);
    bool withinY = (y > m_gridPointY && y < m_gridPointY + m_gridHeight);
    return withinX && withinY;
}

void Gridpoint::setNotEmpty() {
    m_gridPointIsEmpty = false;
}

void Gridpoint::setEmpty() {
    m_gridPointIsEmpty = true;
}

bool Gridpoint::isEmpty() {
    return m_gridPointIsEmpty;
}

Gridpoint::~Gridpoint() {}