/****************************************
 * Filename: EquipBay.hpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for EquipBay
 * Input: 
 * Output:
 * *************************************/

#ifndef EQUIPBAY_HPP
#define EQUIPBAY_HPP

#include "Room.hpp"

class EquipBay : public Room
{
    private:
        bool searchedTwice;
    public:
        EquipBay();
        virtual int interactWithRoom();
        virtual void searchRoom();
        virtual void enterRoom();
        virtual bool getHasItems(); 
};

#endif
