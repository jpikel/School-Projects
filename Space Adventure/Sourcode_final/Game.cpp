/****************************************
 * Filename: Game.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.28
 * Description: Implementation file for Game Class
 * Starts and runs the SSS game
 * Input:
 * Output:
 * *************************************/

#include "Game.hpp"

/****************************************
 * Function: Game::Game()
 * Description: default constructor.  Clears the screen
 * sets up the basics of a game 
 * Parameters: none
 * Preconditions: none
 * Postconditions: Game of SSS instance
 * *************************************/

Game::Game()
{
    gameStatus = 1;
    system("clear");



}

/****************************************
 * Function: Game::~Game()
 * Description: destructor steps through the number
 * of rooms there are and deletes each one
 * Parameters: none
 * Preconditions: Game with rooms
 * Postconditions: Heap memory freed
 * *************************************/

Game::~Game()
{
    cout << endl << "Good-bye!" << endl;
}

/****************************************
 * Function: Game::cleanUp()
 * Description: cleans up the heap memory and deletes
 * the rooms and hero
 * Parameters: none
 * Precondition: ame instance
 * Postconditions: heap memory freed
 * *************************************/

void Game::cleanUp()
{
    currentRoom = first;

    for(int i = 0; i < numberRooms; i++)
    {
        Room *tmp = currentRoom;
        currentRoom = currentRoom->getDoor1();
        delete tmp;
    }
    
    delete hero;
}

/****************************************
 * Function: Game::mainMenu()
 * Description: displays the main Menu and gives 
 * the user the option to start the game or to exit
 * Parameters: None
 * Preconditions: Game Instance
 * Postconditions: game starts or game exits.
 * *************************************/

void Game::mainMenu()
{

    printStars(85);
    cout << "Welcome to the Spherical Space Station."
         << endl << "Please choose from the following options."
         << endl << "Note: please adjust your window so all * are on one line."
         << endl << "1. Start game"
         << endl << "2. Display tips"
         << endl << "3. Display High Scores"
         << endl << "4. Exit"
         << endl;

    switch(valInt(1,4))
    {
        case 1: buildRooms();
                buildRagdoll();
                gameDescription();
                runGame();
                break;
        case 2: displayTips();
                break;
        case 3: readInHighScore();
                printHighScore();
                writeHighScore();
                break; 
        case 4: gameStatus = EXIT;
                break;
    }
}

/****************************************
 * Function: Game::buildRooms()
 * Description: creates 6 rooms and links them together
 * populates the rooms with some items as well.
 * Parameters: none
 * Preconditions: Game instance
 * Post conditions: 6 rooms that are linked
 * *************************************/

void Game::buildRooms()
{
    timer = 60;
    numberRooms = 6;
    currentRoom = 0;
    first = 0;    

    airLockClosed = true;
    roomSwap = true;

    Room *previous;
    currentRoom = new SleepingQuarters(); // Room1
    currentRoom->addItem(new Notepad());
    first = currentRoom;
    previous = currentRoom;
    currentRoom = currentRoom->getDoor1();

    currentRoom = new Laboratory(); // Room2
    currentRoom->addItem(new OxygenTank());
    currentRoom->addItem(new Notepad("1024 is written on the back cover."));
    previous->setDoor1(currentRoom);
    currentRoom->setDoor2(previous);
    previous = currentRoom;
    currentRoom = currentRoom->getDoor1();

    currentRoom = new Bridge(); // Room3
    currentRoom->addItem(new LaserScalpel());
    previous->setDoor1(currentRoom);
    currentRoom->setDoor2(previous);
    previous = currentRoom;
    currentRoom = currentRoom->getDoor1();

    currentRoom = new EquipBay(); // Room4
    currentRoom->addItem(new EnergyCell());
    currentRoom->addItem(new SpaceSuit());
    previous->setDoor1(currentRoom);
    currentRoom->setDoor2(previous);
    previous = currentRoom;
    currentRoom = currentRoom->getDoor1();

    currentRoom = new CargoBay(); // Room5
    currentRoom->addItem(new EnergyCell());
    previous->setDoor1(currentRoom);
    currentRoom->setDoor2(previous);
    previous = currentRoom;
    currentRoom = currentRoom->getDoor1();

    currentRoom = new Truss(); // Room6
    previous->setDoor1(currentRoom);
    currentRoom->setDoor2(previous);
    currentRoom->setDoor1(first);
    first->setDoor2(currentRoom);


    //setup doors 3 and 4 for room 1 i.e. sleeping quarters
    currentRoom = first;
    previous = first;
    currentRoom = currentRoom->getDoor1(); 
    currentRoom = currentRoom->getDoor1(); 
    currentRoom = currentRoom->getDoor1(); 
    previous->setDoor4(currentRoom);
    currentRoom = currentRoom->getDoor1();
    previous->setDoor3(currentRoom);
   
    //setup doors 3 and 4 for room2 i.e. laboratory
    previous = previous->getDoor1();
    currentRoom = currentRoom->getDoor2();
    previous->setDoor3(currentRoom);
    currentRoom = currentRoom->getDoor1(); 
    currentRoom = currentRoom->getDoor1();
    previous->setDoor4(currentRoom);
    
    //setup doors 3 and 4 for room3 i.e. Bridge
    previous = previous->getDoor1();
    previous->setDoor3(currentRoom);
    currentRoom = currentRoom->getDoor2();
    previous->setDoor4(currentRoom);
 
    //setup door 3 and 4 for room4 i.e. EquipBay
    previous = previous->getDoor1();
    previous->setDoor3(first);
    currentRoom = first;
    currentRoom = currentRoom->getDoor1();
    previous->setDoor4(currentRoom);

    //setup doors 3 and 4 for room5 i.e. CargoBay
    previous = previous->getDoor1();
    currentRoom = currentRoom->getDoor1();
    previous->setDoor3(currentRoom);
    previous->setDoor4(first);

    //setup doors 3 and 4 for room6 i.e truss
    previous = previous->getDoor1();
    previous->setDoor4(currentRoom);
    currentRoom = currentRoom->getDoor2();
    previous->setDoor3(currentRoom);

    currentRoom = first;

    system("clear");
}

/****************************************
 * Function: Game::buildRagdoll()
 * Description: asks the user to enter a name for the hero
 * Parameters: none
 * Preconditions: Game instance
 * Postconditions: Ragdoll instance
 * *************************************/

void Game::buildRagdoll()
{
    std::string name;

    cout << endl << "Please enter a name for your hero. If you do not enter a name" << endl
                 << "your hero will be named John Doe (limited to 10 characters)." << endl;

    cin.ignore(500, '\n');
    getline(cin, name);

    if(name.empty())
        hero = new Ragdoll();
    else
    {
        name.assign(name,0, 10);
        hero = new Ragdoll(name);
    }
}

/****************************************
 * Function: Game::valInt(int, int)
 * Description: accepts two ints for a certain range and returns a valid int
 * Parrmeters: int, int
 * Precondtions: Game Instance
 * Postconditions: returns int
 * *************************************/

int Game::valInt(int low, int high)
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

/****************************************
 * Function: Game::gameDescription()
 * Description: prints to screen some instructions and the update!
 * Parameters: none
 * Preconditions: Game Instance
 * Postconditions: display on screen instructions
 * *************************************/

void Game::gameDescription()
{
    cout << endl << "Before starting a few notes."
         << endl << "Every action costs 1 time unit, this includes opening your"
         << endl << "inventory or interacting with a room."
         << endl << "Use your time wisely."
         << endl
         << endl << "You are the only one remaining on the Spherical Space Station, while"
         << endl << "the scientists take a break back on Earth. You've been left"
         << endl << "to maintain their science experiments and keep the place tidy."
         << endl << "One morning you are woken up to the awful sound of ... what"
         << endl << "sounds like gnawing on the hull.  The only thing you know that"
         << endl << "such a distinct sound are the awful ferrous leaches!"
         << endl << "the only way to get them off the hull is to perform a spacewalk!"
         << endl << "Please hit [ENTER] to proceed." << endl;

    cin.get();
}
/****************************************
 * Function: Game::getGameStatus()
 * Description: returns int stored in gameStatus
 * Parameters: none
 * Preconditions: Game instance
 * Postconditions: returns int
 * *************************************/

int Game::getGameStatus()
{
    return gameStatus;
}

/****************************************
 * Function: Game::runGame()
 * Description: runs the game until either the timer
 * is run out or the characters makes a space walk
 * Parameters: none
 * Preconditions: Game Instance
 * Postconditions: game is won or lost
 * *************************************/

void Game::runGame()
{
    do
    {
        printStars(85);
        timeLeft();
        currentRoom->enterRoom();
        printActionMenu();
        printStars(85);
        getUserInput();

        if(timer < 45 && roomSwap)
            timeForAirLock();


        if(timer <= 0)
            endGame();

    }while(airLockClosed);
   
    cleanUp(); 
}

/****************************************
 * Function: Game::printActionMenu()
 * Description: prints to screen the options that a user may enter
 * Parameters: none
 * Preconditions: Game Instance
 * Postconditions:display on screen the possible actions
 * *************************************/

void Game::printActionMenu()
{
    cout << endl << endl << "Possible actions are:" << endl
         << "Doors: '1', '2', '3', or '4'"
         << endl << "Hero : 's'earch, 'i'nventory";

    if(currentRoom->getRoomSearched()) 
        cout << ", inter'a'ct";


    if(currentRoom->getRoomSearched() 
    && currentRoom->getHasItems()
    && currentRoom->getLights())
        cout << ", 'e'xamine items, 'g'et items";

}

/****************************************
 * Function: Game::getUserInput()
 * Description: waits for the user to enter a command
 * and executes the appropriate function for that command
 * Parameters: none
 * Preconditions: Game instance
 * Postconditions: calls the correct function
 * *************************************/

void Game::getUserInput()
{
    char input;
    bool valid = true;
    int tempTimer;

    cout << "Please enter your choice as the letter in ' ': ";

    do
    {
        valid = true;
        input = validateInput();

        switch(input)
        {
            case '1': move(1);
                      break;
            case '2': move(2);
                      break;
            case '3': move(3);
                      break;
            case '4': move(4);
                      break;
            case 's': currentRoom->searchRoom();
                      break;
            case 'e': examineItem();
                      break;
            case 'g': getItem();
                      break;
            case 'i': inventory();
                      break;
            case 'a': tempTimer = currentRoom->interactWithRoom();
                      if(tempTimer == ENDGAME)
                      {
                          airLockClosed = false; 
                          //valid = false;
                          endGame();
                      }
                      else
                          timer += tempTimer;

                      timer++; //counteracts the timer-- below;
                      break;
            default:  cout << "Invalid entry." << endl;
                      valid = false;
                      break;
        }
    }while(!valid);

    timer--;
}

/****************************************
 * Function: Game::validateInput()
 * Description: validates the char the user entered
 * based on certain room conditions, such as whether the 
 * room has been searched or not
 * Parameters: none
 * Preconditions: Game instance with a Room
 * Postconditions: returns valid vhar
 * *************************************/

char Game::validateInput()
{
    char input;
    cin >> input;
    input = tolower(input);

    cin.ignore(500, '\n');

    if(currentRoom->getRoomSearched() 
        && currentRoom->getLights() 
        && currentRoom->getHasItems())
        return input;
    else if(currentRoom->getRoomSearched()
            && currentRoom->getLights())
    {
        if(input == 'e' || input == 'g')
            input = '.';
    }
    else if(currentRoom->getRoomSearched()
            && currentRoom->getName() == "Equipment Bay")
    {
        if(input == 'e' || input == 'g')
            input = '.';
    }
    else
    {
        if(input == 'e' || input == 'g' || input == 'a')
            input = '.';
    }

    return input;
}

/****************************************
 * Function: Game::move(int)
 * Description: attempts to move the character to the 
 * desired room. if this is not possible displays on screen
 * and leaves character in currentRoom
 * Parameters: int (corresponds to the door)
 * Preconditions: Game instance with at least 1 room
 * Postconditions: successful move to new room
 * *************************************/

void Game::move(int door)
{
    if(door == 1 && currentRoom->getDoor1() != NULL)
        currentRoom = currentRoom->getDoor1();
    else if(door == 2 && currentRoom->getDoor2() != NULL)
        currentRoom = currentRoom->getDoor2();
    else if(door == 3 && currentRoom->getDoor3() != NULL)
        currentRoom = currentRoom->getDoor3();
    else if(door == 4 && currentRoom->getDoor4() != NULL)
        currentRoom = currentRoom->getDoor4();
    else
    {
        cout << endl << "You are not able to go that direction." << endl;
        timer++;
    }
}


/****************************************
 * Function: Game::getItem()
 * Description: prints the current items in the room
 * and allows user to select one
 * Parameters: None
 * Preconditions: Room with item
 * Postconditions: item 
 * *************************************/

void Game::getItem()
{
    cout << endl << "Items in the room.";
    currentRoom->printItems();
    cout << endl << "Which Item would you like to pick up? ";
    Item* temp;
    temp = hero->pickUp(currentRoom->removeItem(valInt(1, currentRoom->getNumberItems())));

    if(temp != 0)
        currentRoom->addItem(temp);
}

/****************************************
 * Function: Game::examineItem()
 * Description: allows user to pick up an item temporarily
 * and examine it's details
 * Parameters: none
 * Preconditions: Room with Item
 * Postconditions: 
 * *************************************/
void Game::examineItem()
{
    cout << endl << "Items in the room:";
    currentRoom->printItems();
    cout << endl << "Which item would you like to examine? ";
    currentRoom->printItemDetail(valInt(1, currentRoom->getNumberItems()));
}
/****************************************
 * Function: Game::inventory()
 * Description: opens the characters inventory
 * Parameters: if an item is returned drops it in the room
 * Preconditions: Ragdoll instance
 * Postconditions: Item possibly returned to room
 * *************************************/

void Game::inventory()
{
    Item *tmp = hero->openInventory();
    if(tmp != 0)
        currentRoom->addItem(tmp);
}

/****************************************
 * Function: Game::timeLeft()
 * Description: prints to screen the time left in game
 * Parameters: None
 * Preconditions: Game Instance
 * Postconditions: display on screen the timer
 * *************************************/

void Game::timeLeft()
{
    cout << endl << "Time left: " << timer << " time units." << endl;
}


/****************************************
 * Function: Game::timeForAirlock()
 * Description: checks if the timer has dropped to 45
 * and if so adds an airLock where the truss used to be.
 * Parameters: none
 * Preconditions: a full Game with 6 rooms
 * Postconditions: truss replaced with Airlock
 * *************************************/

void Game::timeForAirLock()
{

    if(!(currentRoom->getName() == "Truss Section"))
    {
        roomSwap = false;
        Room* airlock = first;
        airlock = airlock->getDoor2();

        delete airlock;
        airlock = new Airlock();
        Room *next = first;
        first->setDoor2(NULL);
        airlock->setDoor1(NULL);

        next = next->getDoor1();
        next->setDoor4(airlock);
        airlock->setDoor3(next);

        next = next->getDoor1();
        next->setDoor3(airlock);
        airlock->setDoor4(next);

        next = next->getDoor1();
        next = next->getDoor1();
        next->setDoor1(airlock);
        airlock->setDoor2(next);

        cout << endl << "You hear several bulkhead doors slam shut behind you\n"
                     << "and others open. Very interesting, have the critters gotten"
             << endl << "through?";
    }

}

/****************************************
 * Function: Game::endGame()
 * Description: ends the game. Checks if the character 
 * has the necessary items in his inventory otherwise 
 * character dies in space!
 * Parameters: none
 * Preconditions: Game instance
 * Postconditions: ends the game
 * *************************************/

void Game::endGame()
{
    if(airLockClosed)
    {
        cout << endl << endl << "You have run out of time! The ferrous leaches have made"
             << endl << "too many breaches in the hull and the entire space station, rapidly"
             << endl << "depressurizes.  Resulting in your demise.  Better luck next time.";
    }
    else
    {
        std::string standard;
        standard = "\nThe ferrous leaches breach the hull and you die...";
        cout << endl;
        switch(hero->validateInventory())
        {
            case 1: cout << "Congratulations! You were successful in your task!\n"
                         << "You killed all the leaches and are safely back inside.";
                    readInHighScore();
                    addHighScore();
                    writeHighScore();
                    break;
            case 2: cout << "You did not have enough ammunition, to kill all the leaches"
                         << "\nand there is not enough on board any more. " << standard;
                    break;
            case 3: cout << "You didn't have any ammunition!  There is not enough"
                         << "\noxygen left to perform another spacewalk. " << standard;
                    break;
            case 4: cout << "You didn't take a laser scalpel with you! There is not enough"
                         << "\noxygen left to perform another spacewalk. " << standard;
                    break;
            case 0: cout << "You entered space without a spacesuit. You die...";
                    break;
            default: break;
        }
    }

    airLockClosed = false;
}
        
/****************************************
 * Function: Game::displayTips()
 * Description: provides hints for winning
 * Parameters: none
 * Preconditions: Game instance
 * postconditions: displayh on screen
 * *************************************/

void Game::displayTips()
{
    cout << endl << endl 
         << "In order to win, you need to collect a Space Suit, a Laser Scalpel and\n"
         << "have two Energy Cells in your backpack. " << endl
         << "Search a room first, to interact with the room or get items." << endl
         << "The passcode to the Airlock is 1024, written in the notepad in" << endl 
         << "the laboratory" << endl
         << "The Airlock will appear at 45 time units." << endl
         << "If you go through door 1, in the next room door 2 will get you back" << endl
         << "If you go through door 3, in the next room door 4 will get you back" << endl
         << "and vice versa so through door 2, door 1 returns you and " << endl
         << "entering door 4 returns through door 3" << endl
         << "Except when the airlock appears then you can no longer access door 1" << endl
         << "from the airlock or door2 from the sleeping quarters." << endl 
         << "Make sure not to drop anything on the Truss Section you will loose it."
         << endl << endl;
}

/****************************************
 * Function: Game::recordHighScore()
 * Description: opens highscore.txt and writes the 
 * contents of **heros to the file
 * Parameters: none
 * Preconditions: Game instance
 * Postconditions: high scores written to file
 * *************************************/

void Game::writeHighScore()
{
    remove("highscore.txt");

    std::ofstream outputFile("highscore.txt");
//    outputFile.open("highscore.txt", std::ofstream::out | std::ofstream::trunc);

    if(outputFile.fail())
        cout << endl << "Not able to open highscore.txt" << endl;
    else
    {
        for(int i = 0; i < MAXHIGHSCORE; i++)
        {
            if(heros[i] != 0)
                outputFile << heros[i]->first << endl << heros[i]->second << endl;
        }
        outputFile.close();
    }



    for(int i = 0; i < MAXHIGHSCORE; i++)
        delete heros[i];

    delete [] heros;

}

/****************************************
 * Function: Game::readInHighScore()
 * Description: opens highscore.txt and reads in 
 * names and scores into the array of pointers to pairs 
 * of heros.
 * Parameters: none
 * Preconditions: Game instance and highscore.txt file
 * Postconditions: fills the heros array
 * *************************************/

void Game::readInHighScore()
{

    heros = new std::pair<std::string, int> *[MAXHIGHSCORE];

    for(int i = 0; i < MAXHIGHSCORE; i++)
        heros[i] = 0;

    std::ifstream inputFile;
    inputFile.open("highscore.txt");
    std::string name;
    int number;

    if(inputFile.fail())
        cout << endl << "No high scores set yet." << endl;
    else
    {
        int index = 0;
        while(getline(inputFile, name) && inputFile >> number)
        {
            heros[index] = new std::pair<std::string, int>(name, number);
            index++;
            inputFile.ignore(5, '\n');
                //inputFile.clear();
            
        }
        inputFile.close();
    }

}

/****************************************
 * Function: Game::addHighScore()
 * Description: takes the winners name and points left and
 * adds them to the array of pointers to pairs called heros
 * Parameters: none
 * Preconditions: Game won
 * Postconditions: Hero name and points added to array
 * Citation: code take from C++:Early Objects 8th Edition, Gaddis
 * *************************************/

void Game::addHighScore()
{
//    std::pair<std::string, int> *lastHero = 0;
    std::pair<std::string, int> *currentHero;
    currentHero = new std::pair<std::string, int>(hero->getName(), timer);

    int position = -1;
    int index = 0;
    bool found = false;

//    if(heros[MAXHIGHSCORE-1] != 0)
//        lastHero = heros[MAXHIGHSCORE-1];

    while(index < MAXHIGHSCORE && !found)
    {
        if(heros[index] == 0 || timer >= heros[index]->second)
        {
            found = true;
            position = index;
            cout << endl << "New high score being added!" << endl;
        }
        index++;
    }

    if(found)
    {
        delete heros[MAXHIGHSCORE-1];
        //std::pair<std::string, int> *tempHero;
//        heros[position] = currentHero;
//        position++;
    
        for(int i = MAXHIGHSCORE-1; i > position; i--)
        {
            heros[i] = heros[i-1];
        }
       
        heros[position] = currentHero; 
/*        if(heros[MAXHIGHSCORE-1] == 0)
            heros[MAXHIGHSCORE-1] = lastHero;
        else
            delete lastHero;*/
    }

    if(!found)
        delete currentHero;
}

/****************************************
 * Function: Game::printHighScore()
 * description: displays on screen the current
 * high scores for the game
 * Parameters: noone
 * preconditions: Game instance
 * Postconditions: display on screen high scores if any
 * *************************************/

void Game::printHighScore()
{
    int position = 1;
    cout << endl;
    printStars(85);
    for(int i = 0; i < MAXHIGHSCORE; i++)
    {
        if(heros[i] != 0)
        {
            cout << std::left << std::setw(2) << position << ". " 
                 << std::setw(10) << heros[i]->first << ", " 
                 << heros[i]->second << " points" << endl;
        }
        else
            cout << std::setw(2) << position << ". " << "See your name here!" << endl;

    position++;

    }
}


/****************************************
 * Function: Game::printStars(int)
 * Description: prints * for the int argument passed
 * Parameters: int
 * Preconditions: Game instance
 * Postconditions: display on screen *
 * *************************************/

void Game::printStars(int x)
{
    cout << endl;
    for(int i = 0; i < x; i++)
        cout << "*";

    cout << endl;
}
