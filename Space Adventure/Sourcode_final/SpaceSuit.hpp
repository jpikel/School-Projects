/****************************************
 * Filename: SpaceSuit.hpp 
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for class item
 * SpaceSuits found in the space station that a 
 * character may wear to perform a space walk.  
 * SpaceSuits need to be filled with Oxygen!
 * Input:
 * Ouput:
 * *************************************/

#ifndef SPACESUIT_HPP
#define SPACESUIT_HPP

#include "Item.hpp"

class SpaceSuit : public Item 
{
    private:

    public:
        SpaceSuit();
/*        virtual std::string getName();
        virtual std::string getDescription();
        virtual std::string getInventorySlot();
        virtual std::string getStatus();
        virtual void setStatus(std::string);*/
};

#endif
