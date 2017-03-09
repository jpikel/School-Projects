/****************************************
 * Filename: SpaceSuit.cpp
 * Author: Johannes Pikel
 * Date: 2015.05.25
 * Description; Implementation file for the SpaceSuit 
 * Class
 * Input:
 * Output:
 * *************************************/

#include "SpaceSuit.hpp"

/****************************************
 * Function: SpaceSuit::SpaceSuit()
 * Description: default constructor, creates a new SpaceSuit with some attributes
 * Parameters: none
 * Preconditions:
 * Postconditions: SpaceSuit created
 * *************************************/

SpaceSuit::SpaceSuit()
{
    this->name = "MK II SSS Suit";
    this->description = "A somewhat worn but still in good working order spacesuit.\n";
    this->description += "This suit looks like it will protect you will if were to\n";
    this->description += "perform a space walk.";
    this->inventorySlot = "worn.";
    this->inventorySlotNumber = 0;
//    this->status = "the tank is empty.";
}




