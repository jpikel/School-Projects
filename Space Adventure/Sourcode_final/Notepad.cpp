/****************************************
 * Filename: Notepad.cpp
 * Author: Johannes Pikel
 * Date: 2015.05.25
 * Description; Implementation file for the Notepad 
 * Class
 * Input:
 * Output:
 * *************************************/

#include "Notepad.hpp"
/****************************************
 * Function: Notepad::Notepad()
 * Description: default constructor, creates a standard
 * uninteresting science notepad
 * Parameters: none
 * Preconditions:
 * Postconditions: Notepad created
 * *************************************/

Notepad::Notepad()
{
    this->name = "Tattered notepad";
    this->description = "A lot of documentation and comments about ";
    this->description += "science experiments.";
    this->inventorySlot = "stored in your pack.";
    this->inventorySlotNumber = 2;
//    this->status = "not of much interest at the moment.";
}


/****************************************
 * Function: Notepad::Notepad(std::string)
 * Description: Creates a notepad and adds additional description
 * Parameters: string
 * Preconditions:
 * Postconditions: Notepad created
 * *************************************/

Notepad::Notepad(std::string extraInformation)
{
    this->name = "Tattered notepad";
    this->description = "A lot of documentation and comments about ";
    this->description += "science experiments.\n";
    this->description += extraInformation;
    this->inventorySlot = "stored in your pack.";
    this->inventorySlotNumber = 2;
//    this->status = "that the 4 digits may be worth remembering.";
}

