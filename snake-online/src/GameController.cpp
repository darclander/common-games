#include "GameController.hpp"

void GameController::onServerMessage(const std::string &message) {
    std::vector<std::string> inputs = splitString(message, ';');
    std::string command = inputs[0];

    if (command == "ADD_SCORE") { 
        addScoreEvent(inputs); 
    }
}

void GameController::onMessage(const std::string &message) {
    std::vector<std::string> inputs = splitString(message, ';');
    std::string command = inputs[0];

    if (command == "PLAYER_POS") { 
        
    }
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
