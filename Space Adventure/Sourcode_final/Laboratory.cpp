/****************************************
 * Filename: Laboratory.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.26
 * Description: Implementation file for Laboratory
 * Input:
 * Output:
 * *************************************/

#include "Laboratory.hpp"

/****************************************
 * Function: Laboratory::Laboratory()
 * Description: default constructor that sets up the
 * sleeping quarters
 * Parameters: none
 * Preconditions: none
 * Postconditions: Laboratory instance
 * *************************************/

Laboratory::Laboratory() : Room()
{
    this->roomName = "Laboratory";
    this->roomDescription = "a number of different science experiments have been left\n";
    this->roomDescription += "out to run while everyone is away.\n";
    this->roomDescription += "You remember that science was never your thing.";
}

/****************************************
 * Function: Laboratory::interactWithRoom()
 * Description: allows the character for up to 24 time units
 * wasting precious time and making that much closer to depressurizing!
 * Parameters: none
 * Preconditions: none
 * Postconditions: returns int
 * *************************************/


int Laboratory::interactWithRoom()
{
    cout << endl << "Being a pilot, petry dishes have always made you nervous.";
    cout << endl << "Looking around at the odd ameoba shapes floating in the" 
         << endl << "various size containes, some of which have grown quite rapidly"
         << endl << "you wonder if the leaches are a natural thing or if some scientist"
         << endl << "had a run away experiment and they left you hear to clean it up."
         << endl << "The metallic crunching is getting louder, better hurry!";

    return 0;
}
