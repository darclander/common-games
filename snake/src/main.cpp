#include <iostream>
#include <chrono>


#include "Gui.hpp"
#include "Snake.hpp"
#include "Grid.hpp"
#include "Menu.hpp"

typedef std::chrono::high_resolution_clock Clock;

#define FPS 5
#define frameDelay = 1000 / FPS

void fpsCap(Uint32 starting_tick) {
    // '1000 / FPS' is meant to be replaced with 'frameDelay'
    if ((1000 / FPS) > SDL_GetTicks() - starting_tick) {
        SDL_Delay(1000 / FPS - (SDL_GetTicks() - starting_tick));
    }
}

int main(int argc, char **argv) {

    GUI ui = GUI("Snake", 800, 600, 0);
    Grid grid = Grid(ui.getRenderer(), 800, 600, 20);
    Snake snake = Snake(ui.getRenderer(), &grid, 40, 40, 3);


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
            // grid.render();
            // snake.render();
            ui.render(grid);
            ui.render(snake); // Perhaps a better solution?
            snake.update(deltaTime, 250.f);
        }
        
        ui.render();

        auto t3 = Clock::now();
        // See method-description
        deltaTime = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t1).count())/ 1000000.f;
        // fpsCap(startingTick);
    }


    return 0;
}