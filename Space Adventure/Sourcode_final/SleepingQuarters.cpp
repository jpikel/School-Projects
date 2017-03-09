/****************************************
 * Filename: SleepingQuarters.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.26
 * Description: Implementation file for SleepingQuarters
 * Input:
 * Output:
 * *************************************/

#include "SleepingQuarters.hpp"

/****************************************
 * Function: SleepingQuarters::SleepingQuarters()
 * Description: default constructor that sets up the
 * sleeping quarters
 * Parameters: none
 * Preconditions: none
 * Postconditions: SleepingQuarters instance
 * *************************************/

SleepingQuarters::SleepingQuarters() : Room()
{
    this->roomName = "Sleeping Quarters";
    this->roomDescription = "a standard SSS sleeping quarters, with personal effects\n";
    this->roomDescription += "lockers, suspended sleeping bunks, showers and toilets.\n";
    this->roomDescription += "If you are feeling tired you may take a rest in a bunk.";
}

/****************************************
 * Function: SleepingQuarters::interactWithRoom()
 * Description: allows the character for up to 24 time units
 * wasting precious time and making that much closer to depressurizing!
 * Parameters: none
 * Preconditions: none
 * Postconditions: returns int
 * *************************************/


int SleepingQuarters::interactWithRoom()
{
    int timeUnits = 0;
    cout << endl << "How many time units would you like to rest?";
    cout << endl << "You may sleep at most 24 times units." << endl;
    timeUnits = valInt(0, 24);

    cout << endl << "You have rested for " << timeUnits << " time units." << endl
         << "Unforunately the leaches are now even louder and you have less time!" << endl;

    timeUnits *= -1;

    return timeUnits;
}

/****************************************
 * Function: SleepingQuarters::valInt(int, int)
 * Description: accepts two ints for a certain range and returns a valid int
 * Parrmeters: int, int
 * Precondtions: SleepingQuarters Instance
 * Postconditions: returns int
 * *************************************/

int SleepingQuarters::valInt(int low, int high)
{
    int entry = 0;
    bool valid = false;

    do
    {
        cin >> entry;

        if(cin.fail() || !(entry >= low && entry <= high))
        {
            cin.clear();
            cin.ignore(500, '\n');
            cout << endl << "Valid entry from " << low << " to " << high
                 << " :";
            valid = false;
        }
        else
            valid = true;
    }while(!(entry >= low && entry <= high) || !valid);

    return entry;
}
