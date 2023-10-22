#include <iostream>

#include "Gui.hpp"
#include "Snake.hpp"

int main(int argc, char **argv) {

    GUI ui = GUI("LÄBÄF", 800, 600, 0);

    while(ui.getWindowClose()) {
        ui.render();

        ui.update();

        ui.clearRenderer();
    }

    return 0;
}