#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <SDL2/SDL.h>


class Observer {
    public:
        virtual void onEvent(const SDL_Event& event) = 0;
};


class Controller {
    public:

        Controller() {
            // m_controllerThread = std::thread(listener, this);
        }

        ~Controller() {
            // m_controllerThread.join();
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

        void update() {
            listener();
        }

    private:

        void listener() {
            while(SDL_PollEvent(&m_event)) {
                notifyEvent(m_event);
            }
        }

        std::vector<Observer*> observers;
        // std::thread m_controllerThread;
        SDL_Event m_event;
        bool running = true;
};

