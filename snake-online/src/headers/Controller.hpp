#pragma once

#include <iostream>
#include <vector>
#include <thread>


#include <SDL2/SDL.h>


#include "Client_interface.hpp"
#include "Observer.hpp"

class Controller {
    public:

        Controller() {


            getIpAdressAndPort(m_ip, m_port);
            m_client = std::make_unique<TcpCommunication>(m_ip.c_str(), m_port); 

            if (m_client->isConnected()) {
                
                // Allow for client to communicate
                std::thread receiveThread(&Controller::comm_receive, this);
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

            observer->setSignalCallback([this](const std::string &message) {
                this->onObserverSignal(message);
            });
        }

        void onObserverSignal(const std::string &message) {
            std::cout << "Controller received notification: " << message << std::endl;
            notifyMessage(message);
        }

        // Notify all attached observers about an event
        void notifyEvent(const SDL_Event& event) {
            for (auto observer : observers) {
                observer->onEvent(event);
            }
        }

        void notifyMessage(const std::string &message) {
            if(m_client->isConnected()) {
                m_client->send(message.c_str(), message.size());
            }
            for (auto observer : observers) {
                observer->onMessage(message);
            }
        }

        void sendMessage(const std::string &message) {
            if (m_client->isConnected()) {
                m_client->send(message.c_str(), message.size());
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

    protected:
        
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

        virtual void handleInput(const std::string& input) {
            // Default implementation: just notify observers
            for (auto observer : observers) {
                observer->onServerMessage(input);
            }
        }

        void comm_receive() {
            while(m_client->isConnected()) {
                std::string input = "";
                m_client->receive(input);
                std::cout << input << std::endl;
                // std::vector<std::string> parsedInput = splitString(input, ';');

                handleInput(input);
            }
        }

        void listener() {
            while(SDL_PollEvent(&m_event)) {
                notifyEvent(m_event);
            }
        }

    private:

};

