/****************************************
 * Filename: CargoBay.hpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for CargoBay
 * Input: 
 * Output:
 * *************************************/

#ifndef CARGOBAY_HPP
#define CARGOBAY_HPP

#include "Room.hpp"
#include <cstdlib>
#include <ctime>

class CargoBay : public Room
{
    private:
        int uses;

    public:
        CargoBay();
        virtual int interactWithRoom();
};

#endif
