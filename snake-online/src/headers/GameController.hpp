#pragma once

#include <iostream>
#include <memory>

#include "Grid.hpp"
#include "Menu.hpp"
#include "Snake.hpp"
#include "Score.hpp"
#include "Gui.hpp"
#include "Observer.hpp"
#include "Controller.hpp"
#include "comm_util.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WINDOW_MIDDLE_X (WINDOW_WIDTH / 2)
#define WINDOW_MIDDLE_Y (WINDOW_HEIGHT / 2)

struct Game {
    GUI *gui;
    Grid *grid;
    std::unordered_map<int, std::shared_ptr<Snake>> *players{};
    std::unordered_map<std::string, std::shared_ptr<Score>> *scores{};
};

class GameController : public Controller {
    public:
        GameController(Game &game);

        // TODO: Move away from public?
        void handleInput(const std::string& input) override;
        // void onMessage(const std::string &message) override;
        // void onEvent(const SDL_Event& event) override {}

        int getPid();

    private:

        void addScoreEvent(std::vector<std::string> &inputs);
        void addPlayerEvent(std::vector<std::string> &inputs);
        void collectScoreEvent(std::vector<std::string> &inputs);
        void onNewPlayerEvent(std::vector<std::string> &inputs);


        std::shared_ptr<Game> game;
        GUI *m_gui;
        Grid *m_grid;
        std::unordered_map<int, std::shared_ptr<Snake>> *m_players;
        std::unordered_map<std::string, std::shared_ptr<Score>> *m_scores;

        std::function<void(const std::string&)> signalCallback;

        int m_myPid = -1;
};