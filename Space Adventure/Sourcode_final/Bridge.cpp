/****************************************
 * Filename: Bridge.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.26
 * Description: Implementation file for Bridge
 * Input:
 * Output:
 * *************************************/

#include "Bridge.hpp"

/****************************************
 * Function: Bridge::Bridge()
 * Description: default constructor that sets up the
 * sleeping quarters
 * Parameters: none
 * Preconditions: none
 * Postconditions: Bridge instance
 * *************************************/

Bridge::Bridge() : Room()
{
    this->roomName = "Bridge";
    this->roomDescription = "the helm of the station. ";
    this->roomDescription += "This is one of your favorite\n spots to be.";
    this->roomDescription += "Just being in this rooms boosts your spirits some.\n ";
    this->roomDescription += "You enjoy looking out the cupola.";
}

/****************************************
 * Function: Bridge::interactWithRoom()
 * Description: allows the character for up to 24 time units
 * wasting precious time and making that much closer to depressurizing!
 * Parameters: none
 * Preconditions: none
 * Postconditions: returns int
 * *************************************/


int Bridge::interactWithRoom()
{
    cout << endl << "You stare out the cupola at the distant blue green planet."
         << endl << "Oh how good it would be to be back on solid ground again."
         << endl << "Definitely been in space too long."
         << endl << "SUDDENLY one of those metal sucksers starts chomping away"
         << endl << "at the cupola's frame. Best to hurry, you have already lost time!";

    return -5;
}

