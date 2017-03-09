/****************************************
 * Filename: Game.hpp
 * Author: Johannes Pikel
 * Date: 2016.05.28
 * Description: Declaration file for the Game class
 * to start and run the SSS game
 * Input: menu options
 * Output: text to screen about the game
 * *************************************/

#ifndef GAME_HPP
#define GAME_HPP

#include "Ragdoll.hpp"
#include "Room.hpp"
#include "Airlock.hpp"
#include "Bridge.hpp"
#include "CargoBay.hpp"
#include "EquipBay.hpp"
#include "Laboratory.hpp"
#include "SleepingQuarters.hpp"
#include "Truss.hpp"

#include <unistd.h>
#include <fstream>
#include <utility>
#include <iomanip>
#include <stdio.h>

#define EXIT 0
#define MAXHIGHSCORE 10

class Game
{
    private:
        Ragdoll *hero;
        int timer;
        int numberRooms;
        Room *currentRoom;
        Room *first;
        bool airLockClosed;
        bool roomSwap;
        int gameStatus;
        std::pair<std::string, int> **heros;
        void cleanUp();
        void buildRooms();
        void buildRagdoll();
        int valInt(int, int);
        void gameDescription();
        void runGame();
        void printActionMenu();
        void getUserInput();
        char validateInput();
        void move(int);
        void getItem();
        void examineItem();
        void inventory();
        void timeLeft();
        void timeForAirLock();
        void endGame();
        void displayTips();
        void writeHighScore();
        void readInHighScore();
        void addHighScore();
        void printHighScore();
        void printStars(int);

    public:
        Game();
        ~Game();
        void mainMenu();
        int getGameStatus();
};

#endif

