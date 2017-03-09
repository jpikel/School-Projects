/****************************************
 * Filename: EnergyCell.hpp 
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for class item
 * EnergyCells found in the space station that a 
 * character may wear to perform a space walk.  
 * EnergyCells need to be filled with Oxygen!
 * Input:
 * Ouput:
 * *************************************/

#ifndef ENERGYCELL_HPP
#define ENERGYCELL_HPP

#include "Item.hpp"

class EnergyCell : public Item 
{
    private:

    public:
        EnergyCell();
/*        virtual std::string getName();
        virtual std::string getDescription();
        virtual std::string getInventorySlot();
        virtual std::string getStatus();
        virtual void setStatus(std::string);*/
};

#endif
