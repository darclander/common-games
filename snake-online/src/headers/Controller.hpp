#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <SDL2/SDL.h>

#include "Client_interface.hpp"

class Observer {
    public:
        virtual void onEvent(const SDL_Event& event) = 0;
};


class Controller {
    public:

        Controller() {

            // TODO: replace this logic with CONFIG
            std::string colorString = "green";
            m_nickname = "default";
            getIpAdressAndPort(m_ip, m_port);
            m_client = std::make_unique<TcpCommunication>(m_ip.c_str(), m_port); 
            std::cout << "HELLO" << std::endl;
            if (m_client->isConnected()) {
                // Initial communication between server and client
                std::string command = "ADD_NEW_PLAYER;" + m_nickname + ";" + colorString; 
                std::cout << command << std::endl;
                std::string input = "";
                if (m_client->send(command.c_str(), command.size())) {
                    m_client->receive(input);
                    std::vector<std::string> parsedInput = splitString(input, ';');
                    for (auto x : parsedInput) std::cout << x << std::endl;
                    // pid             = stoi(parsedInput[1]);
                    int xPos        = stoi(parsedInput[2]);
                    int yPos        = stoi(parsedInput[3]);
                    int gw          = stoi(parsedInput[4]);
                    int gh          = stoi(parsedInput[5]);
                    // TODO: move this logic to snake and grid on creation
                    // int gridWidth   = stoi(parsedInput[4]);
                    // int gridHeight  = stoi(parsedInput[5]);
                    // grid = Grid(ui.getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, gw, gh);
                    // snake = Snake(ui.getRenderer(), xPos, yPos, &grid, 40, 40, 3, color);
                }

                // Allow for client to communicate
                std::thread receiveThread(&TcpCommunication::receiveData, m_client.get());
                receiveThread.detach();
            }            
        }

        ~Controller() {
            // m_controllerThread.join();
            // delete m_client;
        }

        // Attach an observer to the controller
        void attachObserver(Observer* observer) {
            observers.push_back(observer);
        }

        // Notify all attached observers about an event
        void notifyEvent(const SDL_Event& event) {
            for (auto observer : observers) {
                observer->onEvent(event);
            }
        }

        // Simulate an event in the controller
        void simulateEvent() {
            SDL_Event event;
            event.type = SDL_USEREVENT;
            notifyEvent(event);
        }

        void broadcastNewMenu(int menuid) {
            SDL_Event event;
            event.type = SDL_USEREVENT;
            event.user.code = menuid;
            notifyEvent(event);
        }

        void broadcastEvent(int eventId) {
            SDL_Event event;
            event.type = SDL_USEREVENT;
            event.user.code = eventId;
            notifyEvent(event);
        }

        void update() {
            listener();
        }

    private:
        
        std::unique_ptr<TcpCommunication> m_client; 

        std::string m_nickname;
        std::string m_ip = "127.0.0.1";
        int m_port = 12345;



        std::vector<Observer*> observers;
        // std::thread m_controllerThread;
        SDL_Event m_event;
        bool running = true;

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

        void listener() {
            while(SDL_PollEvent(&m_event)) {
                notifyEvent(m_event);
            }
        }

};

