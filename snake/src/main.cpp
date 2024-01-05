#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <thread>

#include "Gui.hpp"
#include "Snake.hpp"
#include "Grid.hpp"
#include "Menu.hpp"
#include "Score.hpp"
#include "Controller.hpp"

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

enum gameState {START_MENU  = 0, 
                GAME_PLAY   = 1,
                OPTIONS     = 2, 
                GAME_QUIT   = 3};

std::string gameStateToString(gameState value) {
    switch (value) {
        case START_MENU:
            return "START_MENU";
        case GAME_PLAY:
            return "GAME_PLAY";
        case OPTIONS:
            return "OPTIONS";
        case GAME_QUIT:
            return "GAME_QUIT";
        default:
            return "UNKNOWN_GAME_STATE";
    }
}

void menuHandler(Menu &menu, gameState &state, gameState &previousState) {
    menu.render();
    int menuChoice = 0;
    menuChoice = menu.getMenuIndex();
    std::cout << menuChoice << std::endl;
    if(menuChoice > 0) {
        gameState newState = static_cast<gameState>(menuChoice);
        previousState = state;
        state = newState;
        std::cout << gameStateToString(previousState) << " -> " << gameStateToString(state) << std::endl;
    } else if (menuChoice == -1) {
        state = previousState;
    }
}

int main(int argc, char **argv) {

    
    GUI ui = GUI("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    Grid grid = Grid(ui.getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, 20);
    Snake snake = Snake(ui.getRenderer(), &grid, 40, 40, 30);
    Controller controller = Controller();



    double deltaTime = 0;
    uint32_t startingTick = 0;


    int state = START_MENU;
    // gameState state         = START_MENU;
    // gameState previousState = START_MENU;

    Menu startMenu      = Menu(ui.getRenderer(), 0, 0, 0, 100, 100, ui.getFont());
    Menu optionsMenu    = Menu(ui.getRenderer(), 1, 0, 0, 100, 100, ui.getFont());
    int option = 0;
    startMenu.addItem("START GAME", MENU_STATE,  state, GAME_PLAY);
    startMenu.addItem("OPTIONS",    MENU_STATE,  state, OPTIONS);
    startMenu.addItem("QUIT",       MENU_OPTION, option);

    controller.attachObserver(&startMenu);
    controller.attachObserver(&optionsMenu);
    controller.attachObserver(&ui);
    controller.attachObserver(&snake);

    bool hasScore = false;
    Gridpoint *scorePoint;

    Score score = Score(ui.getRenderer(), grid.getGridPointWidth(), grid.getGridPointHeight());

    bool running = true;
    while(ui.getWindowClose() && running) {

        controller.update();


        auto t1 = Clock::now();

        startingTick = SDL_GetTicks();

        ui.clearRenderer();
        ui.update();

        if (state == START_MENU) {
            controller.broadcastNewMenu(0);
            startMenu.render();
            // menuHandler(startMenu, state, previousState);
            // startMenu.render();
            // int menuChoice = 0;
            // menuChoice = startMenu.update(deltaTime, ui.getWindowClose());
            // if(menuChoice > 0) {
            //     gameState newState = static_cast<gameState>(menuChoice);
            //     state = newState;
            // }
            // if(menuChoice < 0) {
            //     state = previousState;
            // }

            // menuChoice--;
            // if(menuChoice == 0) {
            //     state = GAME_PLAY;
            // } else if (menuChoice == 1) {
            //     state = OPTIONS;
            // } else if (menuChoice == 2) {
            //     state = GAME_QUIT;
            //     running = false;
            // }
        } else if (state == OPTIONS) {
            controller.broadcastNewMenu(1);
            // menuHandler(optionsMenu, state, previousState);
            // optionsMenu.render();
            // int menuChoice = 0;
            // menuChoice = optionsMenu.update(deltaTime, ui.getWindowClose());
            
            // if(menuChoice < 0) {
            //     state = previousState;
            // }
        } else if(state == GAME_PLAY) {
            
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
        }
        
        ui.render();

        auto t3 = Clock::now();
        
        // See method-description
        deltaTime = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t1).count())/ 1000000.f;
        // fpsCap(startingTick);
    }


    return 0;
}