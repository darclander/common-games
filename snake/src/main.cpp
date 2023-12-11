#include <iostream>
#include <chrono>
#include <random>
#include <thread>

#include "Gui.hpp"
#include "Snake.hpp"
#include "Grid.hpp"
#include "Menu.hpp"
#include "Score.hpp"

typedef std::chrono::high_resolution_clock Clock;

#define FPS 5
#define frameDelay = 1000 / FPS

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600



void fpsCap(Uint32 starting_tick) {
    // '1000 / FPS' is meant to be replaced with 'frameDelay'
    if ((1000 / FPS) > SDL_GetTicks() - starting_tick) {
        SDL_Delay(1000 / FPS - (SDL_GetTicks() - starting_tick));
    }
}

void updateSnake(Snake &s, double deltaTime, double limit) {
    s.update(deltaTime, limit);
}

std::pair<int, int> getRandomCoordinate() {
    std::random_device rd;   
    std::mt19937 gen(rd());  

    // Define the range for x and y
    std::uniform_int_distribution<int> xDist(0, WINDOW_WIDTH);  
    std::uniform_int_distribution<int> yDist(0, WINDOW_HEIGHT);  

    // Generate random x and y coordinates
    int x = xDist(gen);
    int y = yDist(gen);

    return std::make_pair(x, y);
}

int main(int argc, char **argv) {

    GUI ui = GUI("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    Grid grid = Grid(ui.getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, 20);
    Snake snake = Snake(ui.getRenderer(), &grid, 40, 40, 30);



    double deltaTime = 0;
    uint32_t startingTick = 0;

    enum gameState {START_MENU  = 0, 
                    GAME_PLAY   = 1,
                    OPTIONS     = 2, 
                    GAME_QUIT   = 3};

    gameState state = START_MENU;

    Menu startMenu = Menu(ui.getRenderer(), 0, 0, 100, 100, ui.getFont());
    int option = 0;
    startMenu.addItem("Start game", MENU_OPTION, option);
    startMenu.addItem("Options",    MENU_OPTION, option);
    startMenu.addItem("Quit game",  MENU_OPTION, option);

    bool hasScore = false;
    Gridpoint *scorePoint;

    Score score = Score(ui.getRenderer(), grid.getGridPointWidth(), grid.getGridPointHeight());

    while(ui.getWindowClose()) {
        auto t1 = Clock::now();

        startingTick = SDL_GetTicks();

        ui.clearRenderer();
        ui.update();


        if (state == START_MENU) {
            startMenu.render();
            int menuChoice = 0;
            menuChoice = startMenu.update(deltaTime);
            if (menuChoice) {
                menuChoice--;
                if(menuChoice == 0) {
                    state = GAME_PLAY;
                } else if (menuChoice == 1) {
                    state = OPTIONS;
                }
            }
        } else if(state == GAME_PLAY) {
            // std::thread t1(updateSnake, std::ref(snake), deltaTime, 100.f);
            // grid.render();
            // snake.render();
            // ui.render(grid);
            
            if(!hasScore) {
                std::pair<int, int> pos = getRandomCoordinate();
                std::cout << "X: " << pos.first << " Y: " << pos.second << std::endl;
                scorePoint = grid.getPoint(pos.first, pos.second);
                if(scorePoint != nullptr && scorePoint->isEmpty()) {
                    scorePoint->setScore();
                    score.move(scorePoint->getGridPointX(), scorePoint->getGridPointY());
                    hasScore = true;
                }
            } else {
                score.render();
                if(scorePoint != nullptr) hasScore = scorePoint->hasScore();
                else hasScore = false;
            }

            ui.render(snake); // Perhaps a better solution?

            snake.update(deltaTime, 100.f);
            // t1.join();
        }
        
        ui.render();

        auto t3 = Clock::now();
        // See method-description
        deltaTime = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t1).count())/ 1000000.f;
        // fpsCap(startingTick);
    }


    return 0;
}