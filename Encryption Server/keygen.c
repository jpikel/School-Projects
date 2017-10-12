/****************************************
 * Filename: keygen.c
 * Author: Johannes Pikel
 * Date: 2017.03.01
 * ONID: pikelj
 * Class: CS344-400
 * Assignment: Program 4 - OTP
 * Description: This is the keygen program that will generate a random key 
 * according to the one-time pads sequence, using only capital letters of the 
 * Roman alphabet.
 * *************************************/

#include <time.h>       /* so we may use time to seed rand */
#include <stdlib.h>     /* so we may have access to rand and other standard function */
#include <unistd.h>      /* so we have write capaibilities */

int main(int argc, char* argv[]){

    int i, limit, idx;
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    /* if we did not receive exactly 2 arguments this is incorrect */
    /* write message and exit */
    if(argc != 2){
        write(STDERR_FILENO, "Invalid. Use keygen {int_length}\n", 33);
        exit(1);
    }

    /* convert second argument into integer */
    limit = atoi(argv[1]);

    srand(time(NULL));  /* seed the random number generator */

    /* up to the limit passed in write a random capital letter or the space */
    for( i = 0; i < limit; i++) {
        idx = rand() % 27;
        if(idx == 26)
            write(STDOUT_FILENO, " ", 1);
        else
            write(STDOUT_FILENO, &letters[idx], 1);
    }

    /* finish with a new line*/
    write(STDOUT_FILENO, "\n", 1);

    return(0);
}
