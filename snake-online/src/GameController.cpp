#include "GameController.hpp"


GameController::GameController(Game &game) {
    m_gui = game.gui;
    m_grid = game.grid;
    m_players = game.players;
    m_scores = game.scores;

    // Initial communication between server and client
    // TODO: replace this logic with CONFIG
    std::string colorString = "green";
    std::string m_nickname = "default";
    std::string command = "ADD_NEW_PLAYER;" + m_nickname + ";" + colorString;
    Controller::sendMessage(command);
}

void GameController::handleInput(const std::string &message) {

    std::vector<std::string> inputs = splitString(message, ';');
    std::string command = inputs[0];

    if (command == "ADD_SCORE") { 
        addScoreEvent(inputs); 
    }

    if (command == "SCORE_COLLECTED") {
        collectScoreEvent(inputs);
    }
    
    if (command == "NEW_PLAYER_RESPONSE") {
        onNewPlayerEvent(inputs);
    }

    for (auto observer : observers) {
        observer->onServerMessage(message);
    }
}


// void GameController::onMessage(const std::string &message) {
//     std::vector<std::string> inputs = splitString(message, ';');
//     std::string command = inputs[0];

//     if (command == "PLAYER_POS") { 
        
//     }
// }

void GameController::onNewPlayerEvent(std::vector<std::string> &inputs) {

    m_myPid        = stoi(inputs[1]);
    int xPos        = stoi(inputs[2]);
    int yPos        = stoi(inputs[3]);
    int gw          = stoi(inputs[4]);
    int gh          = stoi(inputs[5]);

    Grid grid = Grid(m_gui->getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, 40, 30);
    *m_grid = grid;
    
    Snake snake = Snake(m_gui->getRenderer(), WINDOW_MIDDLE_X, WINDOW_MIDDLE_Y, m_grid, 40, 40, 3, color::GREEN);
    (*m_players)[m_myPid] = std::move(std::make_shared<Snake>(snake));


    // TODO: move this logic to snake and grid on creation
    // int gridWidth   = stoi(parsedInput[4]);
    // int gridHeight  = stoi(parsedInput[5]);
    // grid = Grid(ui.getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, gw, gh);
    // snake = Snake(ui.getRenderer(), xPos, yPos, &grid, 40, 40, 3, color);

}

void GameController::addScoreEvent(std::vector<std::string> &inputs) {
    std::shared_ptr<Score> score = std::make_shared<Score>(m_gui->getRenderer(), m_gui, m_grid->getGridPointWidth(), m_grid->getGridPointWidth());
    int xPos = std::stoi(inputs[3]);
    int yPos = std::stoi(inputs[4]);
    xPos = ((xPos) * (m_grid->getGridPointWidth())) + 1;
    yPos = ((yPos) * (m_grid->getGridPointHeight())) + 1;

    Gridpoint *gp = m_grid->getPoint(xPos, yPos);
    if(!(gp == nullptr)) {
        gp->setScore();
        score->move(gp->getGridPointX(), gp->getGridPointY());
        std::string key = std::to_string(gp->getGridPointX()) + "," + std::to_string(gp->getGridPointY());
        std::cout << "key: " << key << std::endl;
        (*m_scores)[key] = std::move(score);
    }
}


void GameController::collectScoreEvent(std::vector<std::string> &inputs) {
    int pid = std::stoi(inputs[1]);
    int xPos = std::stoi(inputs[4]);
    int yPos = std::stoi(inputs[5]);

    xPos = ((xPos) * (m_grid->getGridPointWidth())) + 1;
    yPos = ((yPos) * (m_grid->getGridPointHeight())) + 1;
    
    Gridpoint *gp = m_grid->getPoint(xPos, yPos);

    gp->removeScore();

    std::string key = std::to_string(gp->getGridPointX()) + "," + std::to_string(gp->getGridPointY());
    std::cout << "key: " << key << std::endl;
    auto it = m_scores->find(key);
    if (it != m_scores->end()) {
        // Erase the score from the map
        std::cout << "Removed score: " << xPos << "," << yPos << std::endl;
        m_scores->erase(it);
    } else {
        // Handle the case when the key is not found
        std::cout << "Key not found in the map." << std::endl;
    }

    auto iterator = m_players->find(pid);
    if (iterator != m_players->end()) {
        iterator->second->grow();
    }
}

int GameController::getPid() {
    return m_myPid;
}
