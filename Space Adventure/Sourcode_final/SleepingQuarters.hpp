/****************************************
 * Filename: SleepingQuarters.hpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for SleepingQuarters
 * Input: 
 * Output:
 * *************************************/

#ifndef SLEEPINGQUARTERS_HPP
#define SLEEPINGQUARTERS_HPP

#include "Room.hpp"

class SleepingQuarters : public Room
{
    private:
        int valInt(int, int);

    public:
        SleepingQuarters();
        virtual int interactWithRoom();
};

#endif
