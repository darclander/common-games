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

class TaskMove : public Task {
    public:
        TaskMove(std::function<void(int, int)> moveFunction, std::function<int()> getX, std::function<int()> getY, Point endPoint, double endTime) 
        : Task(endTime), m_moveFunction(moveFunction), m_getX(getX), m_getY(getY), m_endPoint(endPoint) { 
            m_ox0 = m_getX();
            m_oy0 = m_getY();
            m_sx = m_endPoint.xPos - m_ox0;
            m_sy = m_endPoint.yPos - m_oy0;
        }

        void update(double dt) override {
            
            // Add deltatime as milliseconds to the counter.
            m_startTime += dt;

            // Calculate the factor for how far along the timeline we have passed.
            float k = (m_startTime / m_endTime);

            if (m_startTime < m_endTime) {
                int mx = m_sx * k;
                int my = m_sy * k;

                m_moveFunction(m_ox0 + mx, m_oy0 + my);
            } else {
                m_isCompleted = true;
            }            
        }

    private:
        std::function<void(int, int)> m_moveFunction;
        std::function<int()> m_getX;
        std::function<int()> m_getY;
        Point m_endPoint;

        int m_sx;
        int m_sy;
        int m_ox0;
        int m_oy0;

};

class TaskDelay : public Task {
    public:

        TaskDelay(std::function<void()> callbackFunction, double delay, double endTime = 0) 
        : Task(endTime), m_callbackFunction(callbackFunction), m_delay(delay), m_endTime(endTime) { }

        void update(double dt) override {
            m_delay -= dt;

            if (((m_delay <= 0) || (m_delay <= m_endTime)) && (m_isCompleted == false)) {
                m_callbackFunction();
                m_isCompleted = true;
            }
        }

    private:
        std::function<void()> m_callbackFunction;      
        double m_delay;
        double m_endTime;

};

class TaskBlink : public Task {
    public:
        TaskBlink(std::function<void()> renderFunction, double swaptime) 
        : Task(0), m_renderFunction(renderFunction), m_swaptime(swaptime) {}

        void update(double dt) override {
            m_counter += dt;

            if(m_counter >= m_swaptime) {
                m_shouldRender = !m_shouldRender;
                m_counter = 0;
            } 

            if(m_shouldRender) {
                m_renderFunction();
            } 
        }

    private:
        std::function<void()> m_renderFunction;
        double m_counter = 0;
        double m_swaptime = 0;
        bool m_shouldRender = true;

};