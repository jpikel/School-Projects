/****************************************
 * Filename: Truss.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.26
 * Description: Implementation file for Truss
 * Input:
 * Output:
 * *************************************/

#include "Truss.hpp"

/****************************************
 * Function: Truss::Truss()
 * Description: default constructor that sets up the
 * sleeping quarters
 * Parameters: none
 * Preconditions: none
 * Postconditions: Truss instance
 * *************************************/

Truss::Truss() : Room()
{
    this->roomName = "Truss Section";
    this->roomDescription = "A hallway that leads to other parts of the station.\n";
}

/****************************************
 * Function: Truss::interactWithRoom()
 * Description:
 * Parameters: none
 * Preconditions: none
 * Postconditions: returns int
 * *************************************/


int Truss::interactWithRoom()
{
    cout << endl << "Not much to see here, just a hallway.";
    return 0; 
}

