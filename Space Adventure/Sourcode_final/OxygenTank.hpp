/****************************************
 * Filename: OxygenTank.hpp 
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for class item
 * OxygenTanks found in the space station that a 
 * character may use to fill their space suit! 
 * Input:
 * Ouput:
 * *************************************/

#ifndef OXYGENTANK_HPP
#define OXYGENTANK_HPP

#include "Item.hpp"

class OxygenTank : public Item 
{
    private:

    public:
        OxygenTank();
/*        virtual std::string getName();
        virtual std::string getDescription();
        virtual std::string getInventorySlot();
        virtual std::string getStatus();
        virtual void setStatus(std::string);*/
};

#endif
