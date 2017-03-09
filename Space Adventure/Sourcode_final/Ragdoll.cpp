/****************************************
 * Filename: Ragdoll.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Implementation file for Ragdoll
 * Input:
 * Output:
 * *************************************/

#include "Ragdoll.hpp"


/****************************************
 * Function: Ragdoll::RagDoll()
 * Description: default constructor creates a John Doe character
 * With a pack size of 2
 * inventory[0] is body worn
 * inventory[1] is held in hands
 * 3 and 4 are in the backpack.
 * Parameters: none
 * Preconditions: none
 * Postconditions: ragdoll instance
 * *************************************/

Ragdoll::Ragdoll()
{
    name = "John Doe";
    inventory = new Item*[INVENTORYSIZE];

    for(int i = 0; i < INVENTORYSIZE; i++)
        inventory[i] = 0;
}

/****************************************
 * Function: Ragdoll::RagDoll(string)
 * Description: constructor creates a named  character
 * With a pack size of 2
 * inventory[0] is body worn
 * inventory[1] is held in hands
 * 3 and 4 are in the backpack.
 * Parameters: none
 * Preconditions: none
 * Postconditions: ragdoll instance
 * *************************************/

Ragdoll::Ragdoll(std::string name)
{
    this->name = name;
    inventory = new Item*[INVENTORYSIZE];

    for(int i = 0; i < INVENTORYSIZE; i++)
        inventory[i] = 0;
}
/****************************************
 * Function: Ragdoll::RagDoll()
 * Description: deletes any items in the Ragdoll's inventory
 * Parameters: none
 * Preconditions: Ragdoll
 * Postconditions: frees heap memory
 * *************************************/

Ragdoll::~Ragdoll()
{
    for(int i = 0; i < INVENTORYSIZE; i++)
    {
        if(inventory[i] != 0)
            delete inventory[i];
    }

    delete [] inventory;
}

/****************************************
 * Function: Ragdoll::getName()
 * Description: returns the Ragdoll's name 
 * Parameters: none
 * Preconditions: Ragdoll instance
 * Postconditions: returns string stored in name
 * *************************************/

std::string Ragdoll::getName()
{
    return name;
}


/****************************************
 * Function: Ragdoll::lookAt()
 * Description: Allows the user to look an item in the
 * Ragdoll's inventory. Provides the item's name and 
 * description
 * Parameters: none
 * Preconditions: Ragdoll instance
 * Postconditions: displays Item description if an item
 * *************************************/

void Ragdoll::lookAt()
{
    cout << endl << "Look at which item? ";
    int entry = 0;
    entry = valInt(1, INVENTORYSIZE);
    if(inventory[entry-1] != 0)
    {
        cout << inventory[entry-1]->getName() << ": "
             << inventory[entry-1]->getDescription() << endl;
    }
    else
        cout << "Nothing to look at." << endl;
}

/****************************************
 * Function: Ragdoll::pickUp(Item*)
 * Description: based on the Item's inventorySlotNumber pick's up
 * the item and put's in a slot if available otherwise returns the item
 * back to where it came from.  If successful returns 0
 * Parameters: Item *
 * Preconditions: pass an Item*
 * Postconditions: return an Item* or 0
 * *************************************/

Item* Ragdoll::pickUp(Item* item)
{

    cout << endl << "You pick up the " << item->getName();

    if(item->getInventorySlotNumber() == 0)
    {
        cout << " and put it on.";
        inventory[0] = item;
    }
    else if(item->getInventorySlotNumber() == 1)
    {
        cout << " and holster it.";
        inventory[1] = item;
    }
    else if(item->getInventorySlotNumber() == -1)
    {
        cout << " ... don't be silly this is way too heavy!" << endl;
        return item;
    }
    else
    {
        for(int i = 2; i < INVENTORYSIZE; i++)
        {
            if(inventory[i] == 0)
            {
                cout << " and store it in your backpack." << endl;
                inventory[i] = item;
                return 0;
            }
        }

        cout << ", but your backpack is full, so you put it back down." << endl;
        return item;
    
    }

    cout << endl;
    return 0;
}


/****************************************
 * Function: Ragdoll::inventory()
 * Description:Prints the characters current inventory
 * Parameters: none
 * Preconditions: Ragdoll instance
 * Postconditions: display on screen the characters current inventory
 * *************************************/

Item* Ragdoll::openInventory()
{
    exitInventory = 1;
    Item* tmp;

    do
    {
        int slot = 1;
        cout << endl << slot << ". you are wearing ";
        if(inventory[0] == 0)
            cout << "clothes.";
        else
            cout << inventory[0]->getName();

        slot++;

        cout << endl << slot << ". you are holding ";
        if(inventory[1] == 0)
            cout << "nothing.";
        else
            cout << inventory[1]->getName();

        cout << endl << "In backpack:" << endl;
        for(int i = 2; i < INVENTORYSIZE; i++)
        {
            slot++;
            cout << slot << ". ";
            if(inventory[i] == 0)
                cout << "empty";
            else
                cout << inventory[i]->getName();

            cout << endl;
        }

        cout << endl << "Would you like to 'l'ook at an item"
                     << ", 'd'rop an item, 'e'xit." << endl;
        tmp = getUserInput();
    }while(exitInventory != EXIT);

    return tmp;
}

/****************************************
 * Function: Ragdoll::getUserInput()
 * Description: waits for user to enter a command and executes 
 * inventory related command.
 * Parameters: none
 * Preconditions: Ragdoll instance
 * Postconditions: executes appropriate command
 * *************************************/

Item* Ragdoll::getUserInput()
{
    char input;
    bool valid = true;

    do
    {
        valid = true;
        cin >> input;

        switch(input)
        {
            case 'l': lookAt();
                      return 0;
                      break;
            case 'd': exitInventory = EXIT;
                      return drop(); 
                      break;
            case 'e': exitInventory = EXIT;
                      return 0;
                      break;
            default:  cout << endl << "Invalid entry." << endl;
                      valid = false;
                      break;
        }
    }while(!valid);

    return 0;
}

/****************************************
 * Function: Ragdoll::drop()
 * Description: allows user to drop an item from the character
 * inventory, returning it to the room
 * Parameters: none
 * Preconditions: Ragdoll with items
 * Postconditions: returns item
 * *************************************/

Item* Ragdoll::drop()
{
    cout << endl << "Drop which item? ";
    int entry = 0;
    entry = valInt(1, INVENTORYSIZE);

    Item *tmp = inventory[entry-1];
    if(tmp == 0)
    {
        cout << "Nothing to drop." << endl;
        return 0;
    }
    else
    {
        inventory[entry-1] = 0;
        return tmp;
    }
    
}
/****************************************
 * Function: Ragdoll::validateInventory()
 * Description: returns true if the character
 * has the correct items in the inventory to win
 * Parameters: none
 * Preconditions: Ragdoll instance
 * Postconditions: return bool
 * *************************************/

int Ragdoll::validateInventory()
{
    if(inventory[0] != 0
        && inventory[1] != 0
        && inventory[2] != 0
        && inventory[2]->getName() == "Epic Inc. Energy Cell"
        && inventory[3] != 0
        && inventory[3]->getName() == "Epic Inc. Energy Cell")
    {
        return 1;
    }
    else if(inventory[0] != 0
        && inventory[1] != 0
        && inventory[2] != 0
        && inventory[2]->getName() == "Epic Inc. Energy Cell")
    {
        return 2;
    }
    else if(inventory[0] != 0
        && inventory[1] != 0)
    {
        return 3;
    }
    else if(inventory[0] != 0)
    {
        return 4;
    }
    else
        return 0;
}

/****************************************
 * Function: Ragdoll::valInt(int, int)
 * Description: accepts two ints for a certain range and returns a valid int
 * Parrmeters: int, int
 * Precondtions: Ragdoll Instance
 * Postconditions: returns int
 * *************************************/

int Ragdoll::valInt(int low, int high)
{
    int entry = 0;

    do
    {
        cin >> entry;

        if(cin.fail() || !(entry >= low && entry <= high))
        {
            cin.clear();
            cin.ignore(500, '\n');
            cout << endl << "Valid entry from " << low << " to " << high
                 << " :";
        }
    }while(!(entry >= low && entry <= high));

    return entry;
}
