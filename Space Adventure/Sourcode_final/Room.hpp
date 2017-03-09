/****************************************
 * Filename: Room.hpp
 * Author: Johannes Pikel
 * Date: 2016.05.25
 * Description: Declaration file for the Room Class
 * Input:
 * Output:
 * *************************************/

#ifndef ROOM_HPP
#define ROOM_HPP

#include <queue>
#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;
using std::cin;

#include "Item.hpp"
#include "SpaceSuit.hpp"
#include "OxygenTank.hpp"
#include "Notepad.hpp"
#include "LaserScalpel.hpp"
#include "EnergyCell.hpp"

class Room
{
    protected:
        std::string roomName;
        std::string roomDescription;
        bool roomSearched;
        bool roomHasItems;
        Room* door1;
        Room* door2;
        Room* door3;
        Room* door4;
        Item **roomItems;
        int numberItemsInRoom;
        void increaseNumberItems();
        void decreaseNumberItems();
        bool LightsOn;

    public:
        Room();
        virtual ~Room();
        void printItems();
        void printItemDetail(int);
        void addItem(Item*);
        Item* removeItem(int);
        virtual void enterRoom();
        virtual void searchRoom();
        virtual bool getHasItems();
        bool getRoomSearched();
        int getNumberItems();
        std::string getName();
        bool getLights();
        virtual int interactWithRoom() = 0;
        Room* getDoor1();
        Room* getDoor2();
        Room* getDoor3();
        Room* getDoor4();
        void setDoor1(Room*);
        void setDoor2(Room*);
        void setDoor3(Room*);
        void setDoor4(Room*);


};

#endif
