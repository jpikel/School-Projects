/****************************************
 * Filename: Airlock.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.26
 * Description: Implementation file for Airlock
 * Input:
 * Output:
 * *************************************/

#include "Airlock.hpp"

/****************************************
 * Function: Airlock::Airlock()
 * Description: default constructor that sets up the
 * sleeping quarters
 * Parameters: none
 * Preconditions: none
 * Postconditions: Airlock instance
 * *************************************/

Airlock::Airlock() : Room()
{
    this->roomName = "Airlock";
    this->roomDescription = "The door is sealed tight.\n";
    this->roomDescription += "There is a keypad to unlock the door.\n";
    this->roomDescription += "Definitely would want a spacesuit to go out there.";

    this->keyCombo = "1024";
}

/****************************************
 * Function: Airlock::interactWithRoom()
 * Description: allows the character for up to 24 time units
 * wasting precious time and making that much closer to depressurizing!
 * Parameters: none
 * Preconditions: none
 * Postconditions: returns int
 * *************************************/


int Airlock::interactWithRoom()
{
    std::string entry;

    cout << endl << "Looking down at the keypad you see the display shows: " 
         << " XXXX"
         << endl << "You may enter a passcode: ";
    cin >> entry;

    if(entry == "1024") 
        return ENDGAME;
    else
    {
        cout << endl << "The keypad resets itself and nothing happens.";
        return 0;
    }
}

