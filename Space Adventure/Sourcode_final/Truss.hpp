/****************************************
 * Filename: Truss.hpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for Truss
 * Input: 
 * Output:
 * *************************************/

#ifndef TRUSS_HPP
#define TRUSS_HPP

#include "Room.hpp"

class Truss : public Room
{
    private:
    public:
        Truss();
        virtual int interactWithRoom();
};

#endif
