#include <iostream>
#include <chrono>
#include <random>
#include <thread>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "Gui.hpp"
#include "Snake.hpp"
#include "Grid.hpp"
#include "Menu.hpp"
#include "Score.hpp"
#include "Controller.hpp"
#include "Soundmanager.hpp"
#include "Client.hpp"

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


std::unordered_map<std::string, std::string> getConfiguration(const std::string& filename) {
    std::unordered_map<std::string, std::string> key_value_pairs;

    std::ifstream input_file(filename);

    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return key_value_pairs;
    }

    // Read each line from the file
    std::string line;
    while (std::getline(input_file, line)) {
        // Use stringstream to split the line into key and value
        std::istringstream line_stream(line);
        std::string key, value;

        // Read key and value separated by ':'
        if (std::getline(line_stream, key, ':') && std::getline(line_stream, value)) {
            // Trim leading and trailing whitespaces
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            // Insert key-value pair into the map
            key_value_pairs[key] = value;
        }
    }

    input_file.close();

    return key_value_pairs;
}

void getIpAdressAndPort(std::string &ip, int &port) {
    std::unordered_map<std::string, std::string> config = getConfiguration("config.txt");

    auto ip_it      = config.find("ip");
    auto port_it    = config.find("port");
    
    if (ip_it   != config.end()) ip = ip_it->second;
    if (port_it != config.end()) port = std::stoi(port_it->second);
}

void receiveData(TcpClient &client) {
    char responseBuffer[1024];
    int resp;
    while (true) {
        // Example: Receiving a response from the server
        resp = client.receive(responseBuffer, sizeof(responseBuffer));
        // Process the received data as needed
        if (resp == 0) return;
    }
}




int main(int argc, char **argv) {
    GUI ui = GUI("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    Grid grid = Grid(ui.getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, 20, 15);
    Snake snake = Snake(ui.getRenderer(), WINDOW_MIDDLE_X, WINDOW_MIDDLE_Y, &grid, 40, 40, 4);
    Controller controller = Controller();
    int volume = 64;
    int playSound = 1;
    SoundManager sound = SoundManager(volume, playSound);
    sound.loadSound("./debug/sounds/s.wav", "song");
    sound.setVolume("song", volume); // 50%
    sound.playSound("song", -1);


    std::string ipAddress = "127.0.0.1";
    int port = 0;

    getIpAdressAndPort(ipAddress, port);

    double deltaTime = 0;
    uint32_t startingTick = 0;

    TcpClient client;
    if (client.connectToServer(ipAddress.c_str(), port)) {
        const char* command = "GET_DATA";
        if (client.send(command, strlen(command))) {
            // Example: Receiving a response from the server
        }

        std::thread receiveThread(receiveData, std::ref(client));
        receiveThread.detach();
    }

    int state = START_MENU;
    // gameState state         = START_MENU;
    // gameState previousState = START_MENU;

    Menu startMenu      = Menu(&controller, ui.getRenderer(), 0, WINDOW_MIDDLE_X - (250 / 2), 
                                                    WINDOW_MIDDLE_Y - (200 / 2), 
                                                    250, 
                                                    200, 
                                                    ui.getFont(), state, START_MENU, START_MENU);
    
    Menu optionsMenu    = Menu(&controller, ui.getRenderer(), 1, WINDOW_MIDDLE_X - (250 / 2), 
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

    Score score = Score(ui.getRenderer(), grid.getGridPointWidth(), grid.getGridPointHeight());

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
        } else if (state == GAME_QUIT) {
            running = false;
        }
        
        ui.render();

        auto t3 = Clock::now();
               
        // See method-description
        deltaTime = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t1).count())/ 1000000.f;
        // fpsCap(startingTick);
    }

    client.closeConnection();
    return 0;
}