#include <iostream>
#include <chrono>


#include "Gui.hpp"
#include "Snake.hpp"
#include "Grid.hpp"

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

    GUI ui = GUI("LÄBÄF", 800, 600, 0);
    Grid grid = Grid(ui.getRenderer(), 800, 600, 20);
    Snake snake = Snake(ui.getRenderer(), &grid, 40, 40, 10);

    

    double deltaTime = 0;
    uint32_t startingTick = 0;
    

    while(ui.getWindowClose()) {
        auto t1 = Clock::now();

        startingTick = SDL_GetTicks();


        ui.clearRenderer();

        ui.update();

        grid.render();

        snake.render();
        snake.update(deltaTime, 250.f);

        ui.render();

        auto t3 = Clock::now();
        // See method-description
        deltaTime = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t1).count())/ 1000000.f;
        // fpsCap(startingTick);
    }


    return 0;
}