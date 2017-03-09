/****************************************
 * Filename: Room.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Implementation file for class Room
 * Input:
 * Output:
 * *************************************/

#include "Room.hpp"

/****************************************
 * Function: Room::Room()
 * Description: default constructor
 * Parameters:sets up a default room
 * Preconditions:
 * Postconditions: Room instance
 * *************************************/

Room::Room()
{
    roomName = "na";
    roomDescription = "none";
    this->door1 = NULL;
    this->door2 = NULL;
    this->door3 = NULL;
    this->door4 = NULL;
    this->roomSearched = false;
    this->roomHasItems = false;
    this->numberItemsInRoom = 0;
    this->LightsOn = true;

    roomItems = new Item*[1];
    roomItems[0] = 0;
    
}

/****************************************
 * Function: Room::~Room()
 * Description: destructor
 * Parameters: deletes any items remaining in the room
 * Preconditions:
 * Postconditions:  frees heap memory
 * *************************************/

Room::~Room()
{
    for(int i = 0; i < numberItemsInRoom; i++)
        delete roomItems[i];

    delete [] roomItems;
}

/***************************************
 * Function: Room:printItems()
 * Description: if the room has items will
 * print only their names
 * Parameters: none
 * Preconditions: Room instance
 * Postconditions: Items of room on screen
 * *************************************/

void Room::printItems()
{
    int itemNumber = 0;

    if(roomHasItems)
    {
        for(int i = 0; i < numberItemsInRoom; i++)
        {
            itemNumber++;
            cout << endl << itemNumber << ". " << roomItems[i]->getName();
        }
    }
    else
        cout << endl << "No items in this room.";

    cout << endl;
}

/****************************************
 * Function: Room::printItemDetail(int)
 * Description: for the int passed prints to screen
 * that items details
 * Parameters: int
 * Preconditions: room with item
 * Postconditions; display on screen item details
 * *************************************/

void Room::printItemDetail(int item)
{
    cout << endl << roomItems[item-1]->getName() << ": "
         << roomItems[item-1]->getDescription() << endl;
}


/****************************************
 * Function: Room:addItem(Item*)
 * Description: adds an Item to the room
 * Parameters: Item pointer
 * Precondtions: Room instance
 * Postconditions: Item added to room storage
 * *************************************/

void Room::addItem(Item* item)
{
    if(roomItems[0] == 0)
    {
        numberItemsInRoom++;
        roomItems[0] = item;
        roomHasItems = true;
    }
    else
    {
        increaseNumberItems();
        roomItems[numberItemsInRoom - 1] = item;
    }

    cout << endl << item->getName() << " dropped in room." << endl;
}

/****************************************
 * Function: Room::increaseNumberItems(int)
 * Description: Changes the number of items allowed
 * in the roomItems array by +1
 * Parameters: none
 * Preconditions: Room Instance
 * Postconditions: roomItems array changed in size
 * *************************************/

void Room::increaseNumberItems()
{
    Item **tempStorage = new Item*[numberItemsInRoom + 1];
    
    for(int i = 0; i < numberItemsInRoom + 1; i++)
        tempStorage[i] = 0;

    for(int i = 0; i < numberItemsInRoom; i++)
        tempStorage[i] = roomItems[i];

    delete [] roomItems;

    numberItemsInRoom++;

    roomItems = new Item*[numberItemsInRoom];

    for(int i = 0; i < numberItemsInRoom; i++)
        roomItems[i] = 0;

    for(int i = 0; i < numberItemsInRoom; i++)
        roomItems[i] = tempStorage[i];

    delete [] tempStorage;
}

/****************************************
 * Function: Room::decreaseNumberItems(int)
 * Description: Changes the number of items allowed
 * in the roomItems array by -1
 * Parameters: none
 * Preconditions: Room Instance
 * Postconditions: roomItems array changed in size
 * *************************************/

void Room::decreaseNumberItems()
{
    if(numberItemsInRoom - 1 < 1)
    {
        roomItems[0] = 0;
        roomHasItems = false;
        numberItemsInRoom--;
    }
    else
    {
        Item **tempStorage = new Item*[numberItemsInRoom - 1];
    
        for(int i = 0; i < numberItemsInRoom - 1; i++)
            tempStorage[i] = 0;

        for(int i = 0, j = 0; j < numberItemsInRoom; i++, j++)
            if(roomItems[j] != 0)
                tempStorage[i] = roomItems[j];
            else
                i--;

        delete [] roomItems;

        numberItemsInRoom--;

        roomItems = new Item*[numberItemsInRoom];

        for(int i = 0; i < numberItemsInRoom; i++)
            roomItems[i] = 0;

        for(int i = 0; i < numberItemsInRoom; i++)
            roomItems[i] = tempStorage[i];

        delete [] tempStorage;
    }
}
/****************************************
 * Function: Room::remove(int)
 * Description: removes an Item from the room and returns the item 
 * Parameters: int used for item selection in array
 * Preconditions: Room instance with roomItems
 * Postconditions: item returned
 * *************************************/

Item* Room::removeItem(int value)
{
    if(roomHasItems)
    {
        Item* temp = roomItems[value-1];
        roomItems[value - 1] = 0;
        decreaseNumberItems();
        return temp;
    }
    else
        cout << endl << "Room has no items." << endl;

    return 0;
}

/****************************************
 * Function: Room::enterRoom()
 * Description; prints to screen room's name
 * and a description
 * Parameters: none
 * Preconditions: Room instance
 * PostConditions: display on screen
 * *************************************/

void Room::enterRoom()
{
    cout << endl << "You are in the " << roomName;

    if(roomSearched && roomHasItems)
    {
        cout << endl << "You have searched this room already and it contains:";
        printItems(); 
    }
}

/****************************************
 * Function: Room::searchRoom()
 * Description: checks to see if there are items in the room
 * as well as displays detailed room information
 * Parameteres: none
 * Precondition: Room instance
 * Postcondition: Room searched with detailed description
 * *************************************/

void Room::searchRoom()
{
    roomSearched = true;

    cout << endl << "Looking around you see " << roomDescription << endl;
    if(roomHasItems)
    {
        cout << "You also find some items.";
        printItems();
    }
}

/***************************************
 * Function: Room:getLaights()
 * Description: returns bool in LightsOn()
 * Parameters: none
 * Preconitions: Room Instance
 * Postconditions: returns bool
 * *************************************/

bool Room::getLights()
{
    return LightsOn;
}
 
/****************************************
 * Function: Room::getName()
 * Description: returns string stored in roomName
 * Parameters; none
 * Preconditions: Room instance
 * Postconditions: return string
 * *************************************/

std::string Room::getName()
{
    return roomName;
}
   
/****************************************
 * Function: Room::getHasItems()
 * Description: returns bool for roomHasItems
 * Parameteres: none
 * Precondition: Room instance
 * Postcondition: returns bool
 * *************************************/

bool Room::getHasItems()
{
    return roomHasItems;
}

/****************************************
 * Function: Room::getRoomSearched()
 * Description: returns bool in roomSearched
 * Parameteres: none
 * Precondition: Room instance
 * Postcondition: returns bool
 * *************************************/

bool Room::getRoomSearched()
{
    return roomSearched;
}

/****************************************
 * Function: Room:getNumberItems()
 * Description: returns int stored in numberItemsInRoom
 * Parameters: none
 * Preconditions: Room with Items;
 * Postocnditions returns int
 * *************************************/

int Room::getNumberItems()
{
    return numberItemsInRoom;
}

/****************************************
 * Function: Room::getDoor1()
 * Description: returns pointer to room connected
 * to door1
 * Parameters: none
 * Preconditions: Room with door1 to next
 * Postconditions: return Room*
 * *************************************/

Room* Room::getDoor1()
{
    return door1;
}

/****************************************
 * Function: Room::getDoor2()
 * Description: returns pointer to room connected
 * to door2
 * Parameters: none
 * Preconditions: Room with door2 to next
 * Postconditions: return Room*
 * *************************************/

Room* Room::getDoor2()
{
    return door2;
}

/****************************************
 * Function: Room::getDoor3()
 * Description: returns pointer to room connected
 * to door3
 * Parameters: none
 * Preconditions: Room with door3 to next
 * Postconditions: return Room*
 * *************************************/

Room* Room::getDoor3()
{
    return door3;
}

/****************************************
 * Function: Room::getDoor4()
 * Description: returns pointer to room connected
 * to door4
 * Parameters: none
 * Preconditions: Room with door4 to next
 * Postconditions: return Room*
 * *************************************/

Room* Room::getDoor4()
{
    return door4;
}

/****************************************
 * Function: Room::setDoor1(Room*)
 * Description: sets pointer to door1 to passed arugment
 * Parameters: Room*
 * Preconditions: at least another Room to point to
 * Postconditions: two rooms are linked
 * *************************************/

void Room::setDoor1(Room* room)
{
    this->door1 = room;
}

/****************************************
 * Function: Room::setDoor2(Room*)
 * Description: sets pointer to door2 to passed arugment
 * Parameters: Room*
 * Preconditions: at least another Room to point to
 * Postconditions: two rooms are linked
 * *************************************/

void Room::setDoor2(Room* room)
{
    this->door2 = room;
}

/****************************************
 * Function: Room::setDoor3(Room*)
 * Description: sets pointer to door3 to passed arugment
 * Parameters: Room*
 * Preconditions: at least another Room to point to
 * Postconditions: two rooms are linked
 * *************************************/

void Room::setDoor3(Room* room)
{
    this->door3 = room;
}

/****************************************
 * Function: Room::setDoor4(Room*)
 * Description: sets pointer to door4 to passed arugment
 * Parameters: Room*
 * Preconditions: at least another Room to point to
 * Postconditions: two rooms are linked
 * *************************************/

void Room::setDoor4(Room* room)
{
    this->door4 = room;
}


