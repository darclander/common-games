#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <SDL2/SDL.h>


class Observer {
    public:
        virtual void onEvent(const SDL_Event& event) = 0;

        void setNotifyState(int newNotifyState) {
            m_notifyState = newNotifyState;
        }

        int getNotifyState() {
            return m_notifyState;
        }

    private:
        int m_notifyState = -1;
};


class Controller {
    public:

        Controller(int state) {
            // m_controllerThread = std::thread(listener, this);
            m_state = state;
        }

        ~Controller() {
            // m_controllerThread.join();
        }

        // Attach an observer to the controller
        void attachObserver(Observer* observer, int notifyState = -1) {
            observer->setNotifyState(notifyState);
            observers.push_back(observer);
        }

        // Notify all attached observers about an event
        void notifyEvent(const SDL_Event& event) {
            m_events.push_back(event);
            // std::cout << std::endl << std::endl;
            for (auto observer : observers) {
                // std::cout << (m_state == observer->getNotifyState()) << std::endl;
                if(m_state == observer->getNotifyState() || observer->getNotifyState() == -1) {
                    // std::cout << observer->getNotifyState() << std::endl;
                    observer->onEvent(event);
                }
            }
        }

        void notifyEventBroadcast(const SDL_Event& event) {
            m_events.push_back(event);
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
            notifyEventBroadcast(event);
        }

        void broadcastEvent(int eventId) {
            SDL_Event event;
            event.type = SDL_USEREVENT;
            event.user.code = eventId;
            notifyEvent(event);
        }

        void update(int state) {
            m_state = state;
            listener();
        }

        std::vector<SDL_Event> getEvents() {
            return m_events;
        }

    private:

        void listener() {
            while(SDL_PollEvent(&m_event)) {
                notifyEvent(m_event);
            }
        }

        std::vector<Observer*> observers;
        std::vector<SDL_Event> m_events;
        // std::thread m_controllerThread;
        SDL_Event m_event;
        bool running = true;
        int m_state;
};

