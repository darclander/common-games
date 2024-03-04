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
#include "Soundmanager.hpp"

typedef std::chrono::high_resolution_clock Clock;

#define FPS 60
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
    gui->loadTexture("berry", "./gfx/berry.png");
}

int main(int argc, char **argv) {
    GUI ui = GUI("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    loadTextures(&ui);

    Grid grid = Grid(ui.getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, 20, 15);
    Snake snake = Snake(ui.getRenderer(), WINDOW_MIDDLE_X, WINDOW_MIDDLE_Y, &grid, 40, 40, 5);
    Controller controller = Controller();
    int volume = 64;
    int playSound = 1;
    SoundManager sound = SoundManager(volume, playSound);
    sound.loadSound("./sfx/s1.wav", "song");
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
    std::function<void()> toggleSound = bindMemberFunction(sound, &SoundManager::toggleSound);
    optionsMenu.addItemBar("sound", funcL, funcR);
    optionsMenu.addItemToggle("sound: ", toggleSound);
    optionsMenu.addItemState("asd", 1);

    // int soundVolume;
    // optionsMenu.addItem("sound: ", MENU_ON_OFF, playSound);
    // optionsMenu.addItem("sound", MENU_BAR, volume);

    controller.attachObserver(&startMenu);
    controller.attachObserver(&optionsMenu);
    controller.attachObserver(&ui);
    controller.attachObserver(&snake);
    controller.attachObserver(&sound);

    bool hasScore = false;
    Gridpoint *scorePoint;

    Score score = Score(&ui, grid.getGridPointWidth(), grid.getGridPointHeight());

    int s = 0;
    ui.loadFont("regular_18", "./font.ttf", 18);
    ui.loadFont("regular_108", "./font.ttf", 108);
    Text t = ui.createText("Score: " + std::to_string(s), 75, 50, g_color::WHITE, "regular_18");
    Text game_over = ui.createText("GAME OVER!", 0, 0, g_color::RED, "regular_108");
    ui.updateTextPos(game_over, WINDOW_MIDDLE_X - game_over.width / 2, WINDOW_MIDDLE_Y - game_over.height / 2);

    bool running = true;
    bool isAlive = true;
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
            // ui.render(grid);
            if(!hasScore) {
                std::pair<int, int> pos = getRandomCoordinate();
                std::cout << "X: " << pos.first << " Y: " << pos.second << std::endl;
                std::cout << snake.size() << std::endl;
                scorePoint = grid.getPoint(pos.first, pos.second);
                if(scorePoint != nullptr && scorePoint->isEmpty()) {
                    scorePoint->setScore();
                    score.move(scorePoint->getGridPointX(), scorePoint->getGridPointY());
                    hasScore = true;
                }
            } else {
                score.render();
                if(scorePoint != nullptr) {
                    hasScore = scorePoint->hasScore();
                } else {
                    hasScore = false;
                }

                // TODO: Bad solution, remove
                if(!hasScore) {
                    s += 20;
                    ui.updateTextValue(t, "Score: " + std::to_string(s));
                }
            }

            ui.render(snake); // Perhaps a better solution?
            ui.render(t);

            if(isAlive) {
                isAlive = snake.update(deltaTime, 125.f);    
            } else {
                ui.render(game_over);
            }
            
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
