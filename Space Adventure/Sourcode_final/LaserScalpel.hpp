/****************************************
 * Filename: LaserScalpel.hpp 
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for class item
 * LaserScalpels found in the space station that a 
 * character may wear to perform a space walk.  
 * LaserScalpels need to be filled with Oxygen!
 * Input:
 * Ouput:
 * *************************************/

#ifndef LASERSCALPEL_HPP
#define LASERSCALPEL_HPP

#include "Item.hpp"

class LaserScalpel : public Item 
{
    private:

    public:
        LaserScalpel();
/*        virtual std::string getName();
        virtual std::string getDescription();
        virtual std::string getInventorySlot();
        virtual std::string getStatus();
        virtual void setStatus(std::string);*/
};

#endif
