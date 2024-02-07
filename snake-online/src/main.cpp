#include <iostream>
#include <vector>
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

#define SEND_LIMIT 100

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

void getIpAdressAndPort(std::string &ip, int &port) {
    std::unordered_map<std::string, std::string> config = getConfiguration("config.txt");

    auto ip_it      = config.find("ip");
    auto port_it    = config.find("port");
    
    if (ip_it   != config.end()) ip = ip_it->second;
    if (port_it != config.end()) port = std::stoi(port_it->second);
}

void getName(std::string &name) {
    std::unordered_map<std::string, std::string> config = getConfiguration("config.txt");

    auto name_it = config.find("name");

    if (name_it != config.end()) name = name_it->second;
}

void getColor(GUI &ui, SDL_Color &color, std::string &colorString) {
    std::unordered_map<std::string, std::string> config = getConfiguration("config.txt");

    auto color_it = config.find("color");

    if (color_it != config.end()) {
        color = ui.getColor(color_it->second);
        colorString = color_it->second;
    } 
}


void receiveData(TcpClient &client) {
    char responseBuffer[1024];
    int resp;
    std::string input;
    while (true) {
        // Example: Receiving a response from the server
        
        resp = client.receive(input);
        // Process the received data as needed
        std::unordered_map<std::string, std::string> recvData = parseInput(input);

        for (auto &d : recvData) {
            if(d.first == "NEW_PLAYER") {
                // addNewPlayer();
            }
        }

        if (resp == 0) return;
    }
}

int main(int argc, char **argv) {
    GUI ui = GUI("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    ui.loadTexture("berry", "./textures/berry.png");
    
    Controller controller = Controller();

    int volume = 64;
    int playSound = 1;
    SoundManager sound = SoundManager(volume, playSound);
    sound.loadSound("./debug/sounds/s.wav", "song");
    sound.setVolume("song", volume); // 50%
    sound.playSound("song", -1);

    std::unordered_map<int, std::shared_ptr<Snake>> players{};
    std::unordered_map<std::string, std::shared_ptr<Score>> scores{};
    
    int pid = -1;
    std::string ipAddress = "127.0.0.1";
    int port = 0;
    std::string nickname = "default";
    SDL_Color color = color::GREEN;
    std::string colorString = "green";
    float sendTimer = 0;

    getIpAdressAndPort(ipAddress, port);
    getName(nickname);
    getColor(ui, color, colorString);

    double deltaTime = 0;
    uint32_t startingTick = 0;

    Grid grid = Grid(ui.getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, 40, 30);
    Snake snake = Snake(ui.getRenderer(), WINDOW_MIDDLE_X, WINDOW_MIDDLE_Y, &grid, 40, 40, 3, color);

    TcpClient client = TcpClient(ui.getRenderer(), &ui, &grid, &players, &scores);

    if (client.connectToServer(ipAddress.c_str(), port)) {

        // Initial communication between server and client
        std::string command = "ADD_NEW_PLAYER;" + nickname + ";" + colorString; 
        std::cout << command << std::endl;
        std::string input = "";
        if (client.send(command.c_str(), command.size())) {
            client.receive(input);
            std::vector<std::string> parsedInput = splitString(input, ';');
            for (auto x : parsedInput) std::cout << x << std::endl;
            pid             = stoi(parsedInput[1]);
            int xPos        = stoi(parsedInput[2]);
            int yPos        = stoi(parsedInput[3]);
            int gw          = stoi(parsedInput[4]);
            int gh          = stoi(parsedInput[5]);
            client.setPid(pid);
            // int gridWidth   = stoi(parsedInput[4]);
            // int gridHeight  = stoi(parsedInput[5]);
            grid = Grid(ui.getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, gw, gh);
            snake = Snake(ui.getRenderer(), xPos, yPos, &grid, 40, 40, 3, color);
        }

        // Allow for client to communicate
        std::thread receiveThread(&TcpClient::receiveData, std::ref(client));
        receiveThread.detach();
    }

    std::shared_ptr<Snake> player1 = std::make_shared<Snake>(snake);
    players[pid] = player1;

    Snake *player = players[pid].get();

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
    controller.attachObserver(player);
    controller.attachObserver(&sound);

    bool hasScore = false;
    Gridpoint *scorePoint;

    // Score score = Score(ui.getRenderer(), &ui, grid.getGridPointWidth(), grid.getGridPointHeight());

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

            for (const auto& pair : players) {
                pair.second->render();
            }

            for (const auto &score : scores) {
                score.second->render();
            }



            // if(!hasScore) {
            //     std::pair<int, int> pos = getRandomCoordinate();
            //     std::cout << "X: " << pos.first << " Y: " << pos.second << std::endl;
            //     scorePoint = grid.getPoint(pos.first, pos.second);
            //     if(scorePoint != nullptr && scorePoint->isEmpty()) {
            //         scorePoint->setScore();
            //         score.move(scorePoint->getGridPointX(), scorePoint->getGridPointY());
            //         hasScore = true;
            //     }
            // } else {
            //     score.render();
            //     if(scorePoint != nullptr) hasScore = scorePoint->hasScore();
            //     else hasScore = false;
            // }
            
            // ui.render(*player); // Perhaps a better solution?

            // snake.update(deltaTime, 100.f);
            player->update(deltaTime, 100.f);
        } else if (state == GAME_QUIT) {
            running = false;
        }
        
        ui.render();

        auto t3 = Clock::now();
               
        // See method-description
        deltaTime = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t1).count()) / 1000000.f;

        // fpsCap(startingTick);
        
        int gridXPos = 0;
        int gridYPos = 0;
        Gridpoint *gp = grid.getPoint(player->getPosX(), player->getPosY());
        gridXPos = (gp->getGridPointX() / grid.getGridPointWidth());
        gridYPos = (gp->getGridPointY() / grid.getGridPointHeight());
        std::string command = "PLAYER_UPDATE_POSITION;" + std::to_string(pid) + ";" + std::to_string(gridXPos) + ";" + std::to_string(gridYPos) + ";";
        if(client.isConnected()) client.send(command.c_str(), command.size()); 


    }

    client.closeConnection();
    return 0;
}