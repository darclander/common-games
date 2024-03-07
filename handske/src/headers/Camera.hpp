#pragma once

#include <iostream>

class Camera {
    public:
        Camera() {

        }

        int getXPos() {
            return m_xPos;
        }

        int getYPos() {
            return m_yPos;
        }

        bool updatePos(int newX, int newY) {
            m_xPos = newX;
            m_yPos = newY;
            return true; // For error checking later?
        }


    private:
        int m_xPos;
        int m_yPos;    
};