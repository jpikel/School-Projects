/****************************************
 * Filename: Airlock.hpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for Airlock
 * Input: 
 * Output:
 * *************************************/

#ifndef AIRLOCK_HPP
#define AIRLOCK_HPP

#include "Room.hpp"

#define ENDGAME 99

class Airlock : public Room
{
    private:
        std::string keyCombo;
    public:
        Airlock();
        virtual int interactWithRoom();
};

#endif
