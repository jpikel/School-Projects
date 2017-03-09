/****************************************
 * Filename: EnergyCell.cpp
 * Author: Johannes Pikel
 * Date: 2015.05.25
 * Description; Implementation file for the EnergyCell 
 * Class
 * Input:
 * Output:
 * *************************************/

#include "EnergyCell.hpp"

/****************************************
 * Function: EnergyCell::EnergyCell()
 * Description: default constructor, creates a new EnergyCell with some attributes
 * Parameters: none
 * Preconditions:
 * Postconditions: EnergyCell created
 * *************************************/

EnergyCell::EnergyCell()
{
    this->name = "Epic Inc. Energy Cell";
    this->description = "A small cartridge that looks like it will fit into\n";
    this->description += "the charge port of a laser scalpel.";
    this->inventorySlot = "stored in your pack.";
    this->inventorySlotNumber = 2;
    //this->status = "to have full capacity.";
}


