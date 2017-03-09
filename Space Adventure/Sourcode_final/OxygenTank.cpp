/****************************************
 * Filename: OxygenTank.cpp
 * Author: Johannes Pikel
 * Date: 2015.05.25
 * Description; Implementation file for the OxygenTank 
 * Class
 * Input:
 * Output:
 * *************************************/

#include "OxygenTank.hpp"

/****************************************
 * Function: OxygenTank::OxygenTank()
 * Description: Default constructor assigns some attibutes to the OxygenTank
 * Parameters: none
 * Preconditions:
 * Postconditions: OxygenTank created
 * *************************************/

OxygenTank::OxygenTank()
{
    this->name = "Standard 40lb O2 Cylinder";
    this->description = "This tank looks like it is full of Oxygen.";
    this->inventorySlot = "left in place, it is too heavy to carry.";
    this->inventorySlotNumber = -1;
//    this->status = "to be full.";
}


