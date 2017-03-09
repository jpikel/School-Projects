/****************************************
 * Filename: Ragdoll.hpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for class Ragdoll
 * Input: 
 * Output:
 * *************************************/

#ifndef RAGDOLL_HPP
#define RAGDOLL_HPP

#include "Item.hpp"

#include <iostream>
using std::cout;
using std::endl;
using std::cin;


#define INVENTORYSIZE 4
#define EXIT 0

class Ragdoll
{
    private:
        std::string name;
        Item **inventory;
        int valInt(int, int);
        Item* getUserInput();
        Item* drop();
        void lookAt();
        int exitInventory;

    public:
        Ragdoll();
        Ragdoll(std::string);
        ~Ragdoll();
        std::string getName();
        Item* pickUp(Item*);
        Item* openInventory();
        int validateInventory();
        
        
        

};

#endif
