#include <iostream>

#include "Gui.hpp"
#include "Snake.hpp"
#include "Grid.hpp"

int main(int argc, char **argv) {

    GUI ui = GUI("LÄBÄF", 800, 600, 0);
    Snake snake = Snake(ui.getRenderer(), 40, 40, 3);

    while(ui.getWindowClose()) {
        ui.clearRenderer();

        ui.update();
        snake.render();

        ui.render();
    }

    return 0;
}