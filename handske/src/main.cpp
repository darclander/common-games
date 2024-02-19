#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <thread>

#include "Gui.hpp"
#include "Grid.hpp"
#include "Menu.hpp"
#include "Score.hpp"
#include "Player.hpp"
#include "Controller.hpp"
#include "Soundmanager.hpp"

typedef std::chrono::high_resolution_clock Clock;

#define FPS 5
#define frameDelay = 1000 / FPS

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WINDOW_MIDDLE_X (WINDOW_WIDTH / 2)
#define WINDOW_MIDDLE_Y (WINDOW_HEIGHT / 2)

void fpsCap(Uint32 starting_tick) {
    // '1000 / FPS' is meant to be replaced with 'frameDelay'
    if ((1000 / FPS) > SDL_GetTicks() - starting_tick) {
        SDL_Delay(1000 / FPS - (SDL_GetTicks() - starting_tick));
    }
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

template <typename ClassType>
std::function<void()> bindMemberFunction(ClassType& object, void (ClassType::*memberFunction)()) {
    return std::bind(memberFunction, std::ref(object));
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

void loadTextures(GUI *gui) {
    // gui->loadTexture("berry", "./textures/berry.png");
    gui->loadTexture("player", "./gfx/player.png");
    gui->loadTexture("wall", "./gfx/wall.png");
    gui->loadTexture("tile", "./gfx/tile.png");

}

int main(int argc, char **argv) {
    GUI ui = GUI("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    loadTextures(&ui);


    Grid grid = Grid(&ui, WINDOW_WIDTH, WINDOW_HEIGHT, 20, 15);
    
    Controller controller = Controller();
    Player p = Player(&ui, "player");
    
    int volume = 64;
    int playSound = 1;
    SoundManager sound = SoundManager(volume, playSound);
    sound.loadSound("./debug/sounds/s.wav", "song");
    sound.setVolume("song", volume); // 50%
    sound.playSound("song", -1);

    double deltaTime = 0;
    uint32_t startingTick = 0;


    int state = START_MENU;
    // gameState state         = START_MENU;
    // gameState previousState = START_MENU;

    Menu startMenu      = Menu(&controller, &ui, 0, WINDOW_MIDDLE_X - (250 / 2), 
                                                    WINDOW_MIDDLE_Y - (200 / 2), 
                                                    250, 
                                                    200, 
                                                    ui.getFont(), state, START_MENU, START_MENU);
    
    Menu optionsMenu    = Menu(&controller, &ui, 1, WINDOW_MIDDLE_X - (250 / 2), 
                                                    WINDOW_MIDDLE_Y - (200 / 2), 
                                                    250, 
                                                    200, 
                                                    ui.getFont(), state, START_MENU, OPTIONS);
    int option = 0;
    startMenu.addItemState("START GAME", GAME_PLAY);
    startMenu.addItemState("OPTIONS",    OPTIONS  );
    startMenu.addItemState("QUIT",       GAME_QUIT);

    std::function<void()> funcL = bindMemberFunction(sound, &SoundManager::decreaseVolume);
    std::function<void()> funcR = bindMemberFunction(sound, &SoundManager::increaseVolume);
    optionsMenu.addItemBar("sound", funcL, funcR);
    optionsMenu.addItemToggle("sound: ", funcL);
    optionsMenu.addItemState("asd", 1);

    // int soundVolume;
    // optionsMenu.addItem("sound: ", MENU_ON_OFF, playSound);
    // optionsMenu.addItem("sound", MENU_BAR, volume);

    controller.attachObserver(&startMenu);
    controller.attachObserver(&optionsMenu);
    controller.attachObserver(&ui);
    // controller.attachObserver(&snake);
    controller.attachObserver(&sound);



    ui.loadFont("regular_18", "./font.ttf", 18);
    ui.loadFont("regular_108", "./font.ttf", 108);


    bool running = true;
    
    while(ui.getWindowClose() && running) {
        
        controller.update();

        auto t1 = Clock::now();
        startingTick = SDL_GetTicks();

        ui.clearRenderer();
        ui.update();
        

        if (state == START_MENU) {
            startMenu.render();
        } else if (state == OPTIONS) {
            optionsMenu.render();
            // sound.setVolume("song", volume);
            // std::cout << volume << std::endl;
        } else if (state == GAME_PLAY) {
            controller.broadcastNewMenu(3);
            ui.render(grid);
            
        } else if (state == GAME_QUIT) {
            running = false;
        }
        
        ui.render();

        auto t3 = Clock::now();
               
        // See method-description
        deltaTime = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t1).count())/ 1000000.f;
        // fpsCap(startingTick);
    }

    return 0;
}