/****************************************
 * Filename: Notepad.hpp 
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for class item
 * Notepads found in the space station that a 
 * character may read and store in their backpack 
 * Input:
 * Ouput:
 * *************************************/

#ifndef NOTEPAD_HPP
#define NOTEPAD_HPP

#include "Item.hpp"

class Notepad : public Item 
{
    private:

    public:
        Notepad();
        Notepad(std::string);
/*        virtual std::string getName();
        virtual std::string getDescription();
        virtual std::string getInventorySlot();
        virtual std::string getStatus();
        virtual void setStatus(std::string);*/
};

#endif
