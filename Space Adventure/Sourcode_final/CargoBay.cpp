/****************************************
 * Filename: CargoBay.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.26
 * Description: Implementation file for CargoBay
 * Input:
 * Output:
 * *************************************/

#include "CargoBay.hpp"

/****************************************
 * Function: CargoBay::CargoBay()
 * Description: default constructor that sets up the
 * sleeping quarters
 * Parameters: none
 * Preconditions: none
 * Postconditions: CargoBay instance
 * *************************************/

CargoBay::CargoBay() : Room()
{
    this->roomName = "Cargo Bay";
    this->roomDescription = "the bay has tools storage, workbenches, spare parts and\n";
    this->roomDescription += "controls for the articulating arm.\n";
    this->roomDescription += "You can try to pick off some of the leaches with the arm.";
    srand(time(NULL)); 
    this->uses = 5;
}

/****************************************
 * Function: CargoBay::interactWithRoom()
 * Description: allows the character for up to 24 time units
 * wasting precious time and making that much closer to depressurizing!
 * Parameters: none
 * Preconditions: none
 * Postconditions: returns int
 * *************************************/


int CargoBay::interactWithRoom()
{
    if(uses > 0)
    {
        int leachesKilled = rand() % 12;
        cout << endl
             << "You take control of the articulating arm and pick off a few leaches." 
             << endl << "You were able to kill " << leachesKilled << " leaches." << endl
             << "You figure you've gained " << leachesKilled/3 << " time units.";

        uses--;   

        return leachesKilled/3;
    }
    else
    {
        cout << endl << "Unfortunately, the leaches have destroyed the articulating arm"
             << endl << "and it is no longer operable.";
        return 0;
    }
}
