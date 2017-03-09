/****************************************
 * Filename: LaserScalpel.cpp
 * Author: Johannes Pikel
 * Date: 2015.05.25
 * Description; Implementation file for the LaserScalpel 
 * Class
 * Input:
 * Output:
 * *************************************/

#include "LaserScalpel.hpp"

/****************************************
 * Function: LaserScalpel::LaserScalpel()
 * Description: default constructor, creates a new LaserScalpel with some attributes
 * Parameters: none
 * Preconditions:
 * Postconditions: LaserScalpel created
 * *************************************/

LaserScalpel::LaserScalpel()
{
    this->name = "MK IV Green 200 watt Laser Scalpel";
    this->description = "A high power, precision cutting tool.\n";
    this->description += "You suspect this may work well against those\n";
    this->description += "ferrous leaches eating at the hull.";
    this->inventorySlot = "held in your hands.";
    this->inventorySlotNumber = 1;
//    this->status = "not charged with energy.";
}


