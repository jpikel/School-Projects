/****************************************
 * Filename: Item.hpp 
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for class item
 * Items are props found in the space station that a 
 * character may pick up store in their backpack, hold in their
 * hands or wear.
 * Input:
 * Ouput:
 * *************************************/

#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>

class Item
{
    protected:
        std::string name;
        std::string description;
        std::string inventorySlot;
        int inventorySlotNumber;
//        std::string status;

    public:
        Item();
        virtual ~Item();
        virtual std::string getName();
        virtual std::string getDescription();
        virtual std::string getInventorySlot();
        virtual int getInventorySlotNumber();
//        virtual std::string getStatus();
//        virtual void setStatus(std::string);
};

#endif
