/****************************************
 * Filename: Item.cpp
 * Author: Johannes Pikel
 * Date: 2015.05.25
 * Description; Implementation file for the Item 
 * Class
 * Input:
 * Output:
 * *************************************/

#include "Item.hpp"

/****************************************
 * Function: Item::Item()
 * Description: Sets all variables to "na"
 * Parameters: none
 * Preconditions: Item instance
 * Postconditions: Item created
 * *************************************/

Item::Item()
{
    name = "na";
    description = "na";
    inventorySlot = "na";
//    status = "na";
}

/****************************************
 * Function: Item::~Item()
 * Description: destructor
 * Parameters: none
 * Preconditions:
 * Postconditions:
 * *************************************/
Item::~Item()
{}


/****************************************
 * Function: Item::getName()
 * Description: returns the string stored in name
 * Parameters: none
 * Preconditions: Item instance
 * Postconditions: returns string
 * *************************************/

std::string Item::getName()
{
    return name;
}

/****************************************
 * Function: Item::getDescription()
 * Description: returns the string stored in description
 * Parameters: none
 * Preconditions: Item instance
 * Postconditions: returns string
 * *************************************/

std::string Item::getDescription()
{
    return description;
}

/****************************************
 * Function: Item::getInventorySlot()
 * Description: returns the string stored in inventorySlot
 * Parameters: none
 * Preconditions: Item instance
 * Postconditions: returns string
 * *************************************/

std::string Item::getInventorySlot()
{
    return inventorySlot;
}

/****************************************
 * Function: Item::getInventorySlotNumber()
 * Description: returns the int stored in inventorySlotNumber
 * 0 = worn
 * 1 = hands
 * 2 = backpack
 * Parameters: none
 * Preconditions: Item instance
 * Postconditions: returns string
 * *************************************/

int Item::getInventorySlotNumber()
{
    return inventorySlotNumber;
}
/****************************************
 * Function: Item::getStatus()
 * Description: returns the string stored in status
 * Parameters: none
 * Preconditions: Item instance
 * Postconditions: returns string
 * *************************************/
/*
std::string Item::getStatus()
{
    return status;
}
*/
/****************************************
 * Function: Item::setStatus()
 * Description: sets string status to argument passed
 * Parameters: string
 * Preconditions: Item instance
 * Postconditions: returns string
 * *************************************/
/*
void Item::setStatus(std::string input)
{
    status = input;
}*/
