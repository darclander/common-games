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
#include "Task.hpp"

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
                GAME_QUIT   = 3,
                STARTUP     = 4,
                GAME_ABOUT  = 5};

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

void guiLoadTextures(GUI *gui) {
    gui->loadTexture("berry", "./gfx/berry.png");
}

void guiLoadFonts(GUI *gui) {
    gui->loadFont("regular_18", "./font.ttf", 18);
    gui->loadFont("regular_32", "./font.ttf", 32);
    gui->loadFont("regular_108", "./font.ttf", 108);
}

void initGui(GUI *gui) {
    guiLoadTextures(gui);
    guiLoadFonts(gui);
}

void loadSounds(SoundManager *sm) {
    sm->loadSound("./sfx/song.wav", "song");

    sm->loadSound("./sfx/menu_up.wav", "menu_up");

    // sm->loadSound("./sfx/menu_down.wav", "menu_down");
    sm->loadSound("./sfx/menu_up.wav", "menu_down");

    sm->loadSound("./sfx/menu_down.wav", "options_change");

    sm->loadSound("./sfx/start.wav", "startup");

    sm->loadSound("./sfx/gameover.wav", "gameover");

    sm->loadSound("./sfx/chomp.wav", "score");
}

void reset(Grid &grid, int &score, Snake &snake, bool &alive) {
    score = 0;
    snake.reset();
    alive = true;
    grid.reset();
}

Menu createStartMenu(Controller &c, GUI &gui, SoundManager &sm, int &state) {
    Menu startMenu = Menu(&c, &gui, &sm, 0, 
                        WINDOW_MIDDLE_X - (250 / 2), 
                        WINDOW_MIDDLE_Y - (200 / 2), 
                        250, 
                        200, 
                        gui.getFont(), state, START_MENU, START_MENU);

    startMenu.addItemState("START GAME", GAME_PLAY);
    startMenu.addItemState("OPTIONS", OPTIONS);
    startMenu.addItemState("ABOUT", GAME_ABOUT);
    startMenu.addItemState("QUIT", GAME_QUIT);

    return startMenu;
}

Menu createOptionsMenu(Controller &c, GUI &gui, SoundManager &sm, int &state) {
    Menu optionsMenu = Menu(&c, &gui, &sm, 1, 
                        WINDOW_MIDDLE_X - (250 / 2), 
                        WINDOW_MIDDLE_Y - (200 / 2), 
                        250, 
                        200, 
                        gui.getFont(), state, START_MENU, OPTIONS);

    std::function<void()> funcL = bindMemberFunction(sm, &SoundManager::decreaseVolume);
    std::function<void()> funcR = bindMemberFunction(sm, &SoundManager::increaseVolume);
    std::function<void()> toggleSound = bindMemberFunction(sm, &SoundManager::toggleSound);
    optionsMenu.addItemBar("sound", funcL, funcR);
    optionsMenu.addItemToggle("sound: ", toggleSound);

    return optionsMenu;
}

Menu createAboutMenu(Controller &c, GUI &gui, SoundManager &sm, int &state) {
    const int baseY = WINDOW_MIDDLE_Y - WINDOW_HEIGHT / 5;
    const int offsetY = 32 + 32 / 6;
    
    Menu aboutMenu = Menu(&c, &gui, &sm, 2, 
                        WINDOW_MIDDLE_X - (250 / 2), 
                        baseY + offsetY * 5, 
                        250, 
                        200, 
                        gui.getFont(), state, START_MENU, GAME_ABOUT);
    
    aboutMenu.addItemState("Back", START_MENU); // TODO: Triggers instantly.

    aboutMenu.addText(gui.createTextCentralized("Creator: Darclander", 
        WINDOW_MIDDLE_X, baseY + offsetY*0, g_color::BLUE, "regular_32"));

    aboutMenu.addText(gui.createTextCentralized("Co-creator: Kumole", 
        WINDOW_MIDDLE_X, baseY + offsetY*1, g_color::BLUE, "regular_32"));
    
    aboutMenu.addText(gui.createTextCentralized("Sound effects: Isak", 
        WINDOW_MIDDLE_X, baseY + offsetY*2, g_color::BLUE, "regular_32"));

    return aboutMenu;
}

void notifyStateChange(SoundManager &sm, gameState oldState, gameState state) {
    std::cout << gameStateToString(oldState) << " -> " << gameStateToString(state) << std::endl;
    if(state != GAME_PLAY) {
        sm.stopSound("song");
    }

    if (state == GAME_PLAY) {
        sm.playSound("song", -1);
    }
}

int main(int argc, char **argv) {
    GUI ui = GUI("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    initGui(&ui);

    Grid grid = Grid(ui.getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, 20, 15);
    

    int state = STARTUP;
    int oldState = STARTUP;
    Controller controller = Controller(state);
    int volume = 50;
    int playSound = 1;
    SoundManager sound = SoundManager(volume, playSound);
    loadSounds(&sound);
    sound.setVolume(volume); // 50%
    sound.setVolumeAll();
    // sound.playSound("song", -1);

    Snake snake = Snake(ui.getRenderer(), &sound, WINDOW_MIDDLE_X, WINDOW_MIDDLE_Y, &grid, 40, 40, 5);

    double deltaTime = 0;
    uint32_t startingTick = 0;

    // gameState state         = START_MENU;
    // gameState previousState = START_MENU;

    Menu startMenu = createStartMenu(controller, ui, sound, state);
    
    Menu optionsMenu = createOptionsMenu(controller, ui, sound, state);

    Menu aboutMenu = createAboutMenu(controller, ui, sound, state);
    
    
    int option = 0;

    // int soundVolume;
    // optionsMenu.addItem("sound: ", MENU_ON_OFF, playSound);
    // optionsMenu.addItem("sound", MENU_BAR, volume);

    // TODO: update so some observers are conditional. Ui should always be notified but the rest should on condition.
    controller.attachObserver(&startMenu, START_MENU);
    controller.attachObserver(&optionsMenu, OPTIONS);
    controller.attachObserver(&aboutMenu, GAME_ABOUT);
    controller.attachObserver(&ui);
    controller.attachObserver(&snake, GAME_PLAY);
    controller.attachObserver(&sound);

    bool hasScore = false;
    Gridpoint *scorePoint;

    Score score = Score(&ui, grid.getGridPointWidth(), grid.getGridPointHeight());

    int s = 0;

    Text t = ui.createText("Score: " + std::to_string(s), 75, 50, g_color::WHITE, "regular_18");
    Text startText = ui.createText("S N A K E", 0, 0, g_color::RED, "regular_108");
    ui.updateTextPos(startText, WINDOW_MIDDLE_X - startText.width / 2, WINDOW_HEIGHT + startText.height);

    Text pressStart = ui.createText("Press any key to continue...", 0, 0, g_color::RED, "regular_32");
    ui.updateTextPos(pressStart, WINDOW_MIDDLE_X - pressStart.width / 2, WINDOW_MIDDLE_Y + startText.height);

    Text game_over = ui.createText("GAME OVER!", 0, 0, g_color::RED, "regular_108");
    ui.updateTextPos(game_over, WINDOW_MIDDLE_X - game_over.width / 2, WINDOW_MIDDLE_Y - game_over.height / 2);

    TaskMove tm = TaskMove( std::bind(&Text::updatePos, std::ref(startText), std::placeholders::_1, std::placeholders::_2), 
                            std::bind(&Text::getX, std::ref(startText)), 
                            std::bind(&Text::getY, std::ref(startText)), 
                            Point(WINDOW_MIDDLE_X - startText.width / 2, WINDOW_MIDDLE_Y - startText.height / 2), 500);

    bool running = true;
    bool isAlive = true;
    bool playMusic = false;
    
    TaskDelay td = TaskDelay ([&sound](){sound.playSound("startup");}, 500);
    TaskBlink tb = TaskBlink (std::bind(&Text::render, std::ref(pressStart)), 1000);

    while(ui.getWindowClose() && running) {

        controller.update(state);
    
        auto t1 = Clock::now();
        startingTick = SDL_GetTicks();

        if(!(state == oldState)) {
            notifyStateChange(sound, (gameState)oldState, (gameState)state);
            oldState = state;
        } else {
            // Do nothing, we remain in the same state.
        }

        ui.clearRenderer();
        ui.update();

        if (state == STARTUP) {
            startText.render();
            td.update(deltaTime);
            tm.update(deltaTime);
            tb.update(deltaTime);
            if(controller.getEvents().back().type == SDL_KEYUP) state = START_MENU;
        } else if (state == START_MENU) {
            startMenu.render();
            if(!isAlive) {
                // Reset everything to initial gamestate if we are in the start menu and the snake is dead.
                reset(grid, s, snake, isAlive);
                ui.updateTextValue(t, "Score: " + std::to_string(s)); 
            }
        } else if (state == OPTIONS) {
            optionsMenu.render();
        } else if(state == GAME_ABOUT) {
            aboutMenu.render();
        } else if (state == GAME_PLAY) {
            controller.broadcastNewMenu(3);

            if(!playMusic) {
                playMusic = true;
                sound.playSound("song", -1);
            }

            // ui.render(grid);
            if(!hasScore) {
                std::pair<int, int> pos = getRandomCoordinate();
                // std::cout << "X: " << pos.first << " Y: " << pos.second << std::endl;
                // std::cout << snake.size() << std::endl;
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
