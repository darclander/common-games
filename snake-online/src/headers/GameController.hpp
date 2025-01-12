#pragma once

#include <iostream>

#include "Grid.hpp"
#include "Menu.hpp"
#include "Snake.hpp"
#include "Score.hpp"
#include "Gui.hpp"
#include "Observer.hpp"
#include "Controller.hpp"
#include "comm_util.hpp"


struct Game {
    GUI *gui;
    Grid *grid;
    std::unordered_map<int, std::shared_ptr<Snake>> *players{};
    std::unordered_map<std::string, std::shared_ptr<Score>> *scores{};
};

class GameController : public Observer {
    public:
        GameController(Game &game) {
            m_gui = game.gui;
            m_grid = game.grid;
            m_players = game.players;
            m_scores = game.scores;
        }

        void onServerMessage(const std::string &message) override;
        void onMessage(const std::string &message) override;
        void onEvent(const SDL_Event& event) override {}

    private:

        void addScoreEvent(std::vector<std::string> &inputs);
        void addPlayerEvent(std::vector<std::string> &inputs);

        GUI *m_gui;
        Grid *m_grid;
        std::unordered_map<int, std::shared_ptr<Snake>> *m_players;
        std::unordered_map<std::string, std::shared_ptr<Score>> *m_scores;
};