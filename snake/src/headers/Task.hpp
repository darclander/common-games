#pragma once

#include <iostream>
#include <functional>
#include <cmath>
#include "Gui.hpp"

class Task {
    public: 

        Task(double endTime) : m_endTime(endTime) { }

        virtual void update(double deltaTime) = 0;

    protected:
        bool m_isCompleted = false;

        double m_startTime = 0;
        double m_endTime = 0;

    private:

};

class TaskMove : Task {
    public:
        TaskMove(std::function<void(int, int)> moveFunction, std::function<int()> getX, std::function<int()> getY, Point endPoint, double endTime) 
        : Task(endTime), m_endPoint(endPoint), m_moveFunction(moveFunction), m_getX(getX), m_getY(getY) { }

        void update(double dt) override {
            
            // Add deltatime as milliseconds to the counter.
            m_startTime += dt / 1000;

            // Calculate the factor for how far along the timeline we have passed.
            int k = (m_startTime / m_endTime);

            if (m_startTime < m_endTime) {
                int ox = m_getX();
                int oy = m_getY();
                
                int epx = m_endPoint.xPos;
                int epy = m_endPoint.yPos; 
                int dx = epx - ox; 
                int dy = epy - oy; 
                
                int mx = dx * ((dt + (m_endTime - m_startTime) / 1000) / 1000);
                int my = dy * ((dt + (m_endTime - m_startTime) / 1000) / 1000);
                
                std::cout << ox << std::endl;
                std::cout << epx << std::endl;
                std::cout << mx << std::endl;

                m_moveFunction(ox + mx, oy + my);
            } else {
                m_isCompleted = true;
            }            
        }

    private:
        std::function<void(int, int)> m_moveFunction;
        std::function<int()> m_getX;
        std::function<int()> m_getY;
        Point m_endPoint;

};