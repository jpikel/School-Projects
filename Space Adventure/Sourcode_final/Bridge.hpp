/****************************************
 * Filename: Bridge.hpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for Bridge
 * Input: 
 * Output:
 * *************************************/

#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include "Room.hpp"

class Bridge : public Room
{
    private:
    public:
        Bridge();
        virtual int interactWithRoom();
};

#endif
