/****************************************
 * Filename: EquipBay.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.26
 * Description: Implementation file for EquipBay
 * Input:
 * Output:
 * *************************************/

#include "EquipBay.hpp"

/****************************************
 * Function: EquipBay::EquipBay()
 * Description: default constructor that sets up the
 * sleeping quarters
 * Parameters: none
 * Preconditions: none
 * Postconditions: EquipBay instance
 * *************************************/

EquipBay::EquipBay() : Room()
{
    this->roomName = "Equipment Bay";
    this->roomDescription = "The station's equipment is stored in this room.\n";
    this->roomDescription += "There are various racks and shelves of different\n";
    this->roomDescription += "modules, electronic parts and tools.";
    
    this->LightsOn = false;
    this->searchedTwice = false;

}
/****************************************
 * Function: EquipBay::interactWithRoom()
 * Description: allows the character for up to 24 time units
 * wasting precious time and making that much closer to depressurizing!
 * Parameters: none
 * Preconditions: none
 * Postconditions: returns int
 * *************************************/


int EquipBay::interactWithRoom()
{
    if(!LightsOn)
    {
        cout << endl << "After some fumbling around in the dark you manage to find"
             << endl << "the light switch and turn it on.  Now you may search for items.";
        LightsOn = true;
    }
    else if(LightsOn)
    {
        cout << endl << "You turn the lights off.";
        LightsOn = false;
    }

    return 0;
}

/****************************************
 * Function: EquipBay::searchRoom()
 * Description: checks to see if there are items in the room
 * as well as displays detailed room information
 * Parameteres: none
 * Precondition: Room instance
 * Postcondition: Room searched with detailed description
 * *************************************/

void EquipBay::searchRoom()
{
    roomSearched = true;

    if(LightsOn)
    {
        cout << endl << "Looking around you see " << roomDescription << endl;
        searchedTwice = true;
        if(roomHasItems)
        {
            cout << "You also find some items.";
            printItems();
        }
    }
    else
    {
        cout << endl << "It's too dark to see anything better turn on the lights first."
             << endl;
    }
}
 

/****************************************
 * Function: EquipBay::enterRoom()
 * Description; prints to screen room's name
 * and a description
 * Parameters: none
 * Preconditions: Room instance
 * PostConditions: display on screen
 * *************************************/

void EquipBay::enterRoom()
{
    if(LightsOn)
    {
        cout << endl << "You are in the " << roomName;

        if(roomSearched && searchedTwice && roomHasItems)
        {
            cout << endl << "You have searched this room already and it contains:";
            printItems(); 
        }
    }
    else
    {
        cout << endl << "It's too dark to see anything.";
    }
}

/****************************************
 * Function: EquipBay::getHasItems()
 * Description: returns bool for roomHasItems
 * had to add this functionality to make it seem
 * like there are no items in this room until the room is searched
 * twice
 * Parameteres: none
 * Precondition: Room instance
 * Postcondition: returns bool
 * *************************************/

bool EquipBay::getHasItems()
{
    if(searchedTwice && LightsOn)
        return roomHasItems;
    else
        return false;
}

