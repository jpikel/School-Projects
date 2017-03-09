/****************************************
 * Filename: Laboratory.hpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for Laboratory
 * Input: 
 * Output:
 * *************************************/

#ifndef LABORATORY_HPP
#define LABORATORY_HPP

#include "Room.hpp"

class Laboratory : public Room
{
    private:

    public:
        Laboratory();
        virtual int interactWithRoom();
};

#endif
