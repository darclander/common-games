#include "Game.hpp"

void Game::handleServerEvents(std::vector<std::string> serverEvents) {
    for (auto se : serverEvents) {
        std::vector<std::string> event = splitString(se, ';');
        std::cout << "Handling: " << event[0] << std::endl;
        handleEvent(event);
    }
}

void Game::handleEvent(std::vector<std::string> &event) {
    std::string command = event[0];
    if(command == "ADD_SCORE") {
        int xPos = stoi(event[3]);
        int yPos = stoi(event[4]);
        addScore(xPos, yPos);
    }
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