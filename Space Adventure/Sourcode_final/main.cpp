/****************************************
 * Filename: main.cpp
 * Author: Johannes Pikel
 * Date: 2016.05.30
 * Description: main for the game defend the
 * Spherical Space Station!
 * Input: a user may navigate and interact with the
 * space station
 * Output: descriptions of the surroundings
 * *************************************/


#include "Game.hpp"

int main()
{
    Game *game = new Game();

    do
    {
        game->mainMenu();
    }while(game->getGameStatus() != EXIT);

    delete game;

    return 0;
}
