#include "Game.hpp"

void Game::handleEvents(std::vector<std::string> serverEvents) {
    for (auto se : serverEvents) {
        std::vector<std::string> event = splitString(se, ';');
        std::cout << "Handling: " << event[0] << std::endl;
        handleEvent(event);
    }
}

void Game::handleEvent(std::vector<std::string> &event) {
    std::string command = event[0];

    // TODO: I think this could segfault in the future if not handled properly
    if (command == "NEW_PLAYER_RESPONSE") {
        setupFromServer(event);
        m_serverSetupIsComplete = true;
    }

    if(command == "ADD_SCORE") {
        int xPos = stoi(event[3]);
        int yPos = stoi(event[4]);
        addScore(xPos, yPos);
    }

    if (command == "PLAYER_SCORE_COLLECTED") {
        int xPos = stoi(event[1]);
        int yPos = stoi(event[2]);
        std::string command = "SCORE_COLLECTED;" + std::to_string(m_myPid) + ";0;0;" + std::to_string(xPos) + ";" + std::to_string(yPos); 
        m_gameController->sendMessage(command);
        removeScore(xPos, yPos);
        playerGrow();
    }

    if (command == "NEW_PLAYER_JOINED") {

    }

    if (command == "PLAYER_UPDATE_POSITION") {

    }
}

void Game::setupFromServer(std::vector<std::string> &event) {
    // Set grid size

    // Set snake color and size

    // Set pid
    m_myPid = stoi(event[1]);
}

void Game::removeScore(int xPos, int yPos) {
    Gridpoint *gp = m_grid->getPoint(xPos + 1, yPos + 1);
    if(!(gp == nullptr)) {
        gp->removeScore();
        std::string key = std::to_string(gp->getGridPointX()) + "," + std::to_string(gp->getGridPointY());
        std::cout << "key remove: " << key << std::endl;
        m_scores.erase(key);
    }
}

void Game::playerGrow() {
    m_players[m_myPid]->grow();
}

void Game::addScore(int xPos, int yPos) {
    std::shared_ptr<Score> score = std::make_shared<Score>(m_gui->getRenderer(), m_gui.get(), m_grid->getGridPointWidth(), m_grid->getGridPointWidth());
    xPos = ((xPos) * (m_grid->getGridPointWidth())) + 1;
    yPos = ((yPos) * (m_grid->getGridPointHeight())) + 1;
    Gridpoint *gp = m_grid->getPoint(xPos, yPos);
    if(!(gp == nullptr)) {
        gp->setScore();
        score->move(gp->getGridPointX(), gp->getGridPointY());
        std::string key = std::to_string(gp->getGridPointX()) + "," + std::to_string(gp->getGridPointY());
        std::cout << "key: " << key << std::endl;
        m_scores[key] = (score);
    }
}