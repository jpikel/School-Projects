/* **************************************
 * Filename: pikelj.buildrooms.c
 * Author: Johannes Pikel
 * Date: 2017.01.30
 * ONID: pikelj
 * Class: CS340-400
 * Assignment: Program 2 Adventure
 * Description: This file will generate 7 different room files
 * where each file corrsponds to 1 room with between 3 to 6 connections
 * and a room type.  This will be used to create the dungeon layout 
 * for the pikelj.adventure game to use these files as the dungeon.
 * *************************************/

#include <stdio.h>      //standard input and output library
#include <fcntl.h>      //file control options
#include <stdlib.h>     //general functions library like random
#include <unistd.h>     //include for the getpid function
#include <sys/types.h>  //get file status or directory status
#include <sys/stat.h>
#include <time.h>       //so we can seed rand with current time
#include <string.h>     //so we can use strcmp
#include <assert.h>     //use to confirm, malloc, etc

//define a few constants
#define MINCONS 3   //minimum number of connections per room 
#define MAXCONS 6   //maximum number of connections per room
#define MAXCHAR 64  //pathName at most 64 bytes


//function prototypes
char* makeDir();                    //make directory function
int* selectRooms();                 //choose the random 7 rooms from 10
int** createConnections(int*);      //make the random connections btwn rooms
                                    //param: int* to 7 selected rooms
void createFiles(char*, int**);     //create the files and write to them
                                    //params: char* to file directory
                                    //int** to 2d array of connections



/****************************************
 * Function: main()
 * Parameters: none
 * Description: This will called the selectRooms() function
 * to choose the seven random rooms, then make the directory 
 * pikelj.rooms.PID.  Then create the random Connections between
 * our selected rooms and final create the individual rooms file.
 * Preconditions: none
 * Postcondtions: directory and 7 room files generated
 * *************************************/

int main(void) {
    int i;    
    int* selectedRooms = selectRooms(); //our 7 random rooms of 10
    char* pathName = makeDir();         //create the directory ./pikelj.rooms.PID/
    int** connections = createConnections(selectedRooms); //make the random connections

    free(selectedRooms);    //createConnections copied the room indices so we can 
                            //free this.  malloc in selectRooms()

    createFiles(pathName, connections); //create the files and write to them

    free(pathName); //pathName malloc() from makeDir, free now that we are done

                    //free the connections malloc'd in createConnections
    for(i = 0; i < 7; i++)
        free(connections[i]);

    free(connections);

    return(0);
} 

/****************************************
* Function: make_dir
* Parameters: none
* Description: creates a directory named pikelj.rooms.PID
* where PID is the process ID of this program
* reference for this bit code code from 
* http://stackoverflow.com/questions/34260350/appending-pid-to-filename
* snprintf returns the number of characters written and then can check
* if the returned int is less than 0 or greater than our defined storage
* both of which are bad so we want to exit!
* Postconditions: directory created and returns full path name 
*****************************************/

char* makeDir() {
    char* pathName = malloc(MAXCHAR * sizeof(char));
    char* myDir = "./pikelj.rooms."; //our folder
    int string_len = 0;             //holds return from snprintf
    int status;                     //store return from mkdir
    
    pid_t pid = getpid();

    //concatanate the pathname so it is the current working directory/pikelj.rooms.PID
    string_len = snprintf(pathName, MAXCHAR, "%s%d", myDir, (int) pid);

    //check if an error of 0 zero bytes written or too many bytes written
    assert(string_len > 0 && string_len < MAXCHAR);

    //now we have the path name make the directory with 
    //user read write execute and group read write execute
    //For good measure also allow others to read write
    //If we get a 0 back we were successful and so we also 
    //return the pathName
    status = mkdir(pathName, S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH);
    assert(status == 0);
    return pathName;
}


/****************************************
 * Function: selectRooms()
 * Parameters: none
 * Description: given that each of our rooms is located at a particular index,
 * we can simply selected 7 random ones that are not already in our list out of
 * 10 possible options.  Make sure we check for a duplicate so we don't have any 
 * and then we can return the pointer to the start of this int array.
 * Preconditions: none
 * Postconditions: returns pointer to the start of an int array.
 * *************************************/

int* selectRooms() {
    srand(time(NULL)); // seed the pseudorandom number generator
    int i, j;
    int randNumber;
    int* selectedRooms = malloc(7 * sizeof(int));   //we only want 7 rooms total

    //validate the malloc of selectedRooms
    assert(selectedRooms != 0);

    //initialize our array to all -1
    for ( i = 0; i < 7; i ++)
        selectedRooms[i] = -1;

    i = 0;

    //pick a random number from 0 to 9, so we have our 10 possible rooms
    //with each random number chosen.
    //check if selectedRooms[j] is -1, will take care of inserting our integer
    //into an empty slot first available.
    //The second if statement is used to exit out of the for loop once a new
    //random number is inserted or if it is encountered earlier in the array
    //before we reach and empty location designated by -1.
    while ( i < 7) {
        randNumber = rand() % 10;       
        for ( j = 0; j < 7; j++) {
            if (selectedRooms[j] == -1) {
                selectedRooms[j] = randNumber;
                i = i + 1;
            }
            if (selectedRooms[j] == randNumber)
                j = 7;
        }
    }
    //used for testing what is contained inside the list before returning it
/*    printf("inside loop: ");
    for(i = 0; i < 7; i++)
        printf("%d, ", selectedRooms[i]);

    printf("\n");*/

    return selectedRooms;
}

/****************************************
 * Function: createConnections
 * Parameters: pointer to an int array of size 7
 * Description: using a 2d array of integers such that for each row
 * in the array, [0] is the room index, [1] is the number of connections, and
 * [2] through [7] are the chosen connected rooms.  For each row in the 2d array
 * attempt to create X number of new connections as long as that number of new 
 * connections does not go above 6.
 * For each possible new connection (room) check that the receiving or room to
 * be connected to has less than 6 connections.  Otherwise skip that room and
 * connected to another.
 * Then add the connection to the end of our current room's connections, add
 * our current room as a connection to the receiving room
 * Increment both of their connection counts at index [1] of their respective row.
 * The 2d array has the following format
 *
 * Room Index |  Total Connections | Cntn 1 | Cntn 2 | Cntn 3 | Cntn 4 | Cntn 5 | Cntn 6
 *
 * Preconditions: pass in a pointer to an int array of size 7
 * Postconditions: returns a pointer to the start of a 2d array of ints
 * sized 7 rows x 8 columns
 * *************************************/

int** createConnections(int* selectedRooms){

    int** connections;
    int i, j, k, r;
    int newCons;
    int randRoom;
    srand(time(NULL));  //reseed our number generator for good measure

    //initialize heap memory for the 2d array of connections
    connections = malloc(7 * sizeof(int*));
    for(i = 0; i < 7; i++){
        connections[i] = malloc(8 * sizeof(int));
    }

    //make sure malloc worked
    assert(connections != 0);

    //initialize the connections 2d array first column is the room index
    //second column is set to 0 for number of connections
    //remaining columns are set to -1 for future checking and will
    //store the connected rooms
    for(i = 0; i < 7; i++){
        for(j = 1; j < 8; j++)
            connections[i][j] = -1;
        connections[i][0] = selectedRooms[i];
        connections[i][1] = 0;
    }
    for(i = 0; i < 7; i++){
        while(connections[i][1] < 3){       //make sure we have at least 3 connects.
            //pick a new number of random connections to make that doesn't 
            //exceed our total allowed connections when added to current connections
            do{
                newCons = rand() % MINCONS + MINCONS;
            } while(connections[i][1] + newCons > MAXCONS); 
            //attempt to make this many new connections.
            for(j = 0; j < newCons; j++){
                do{
                    randRoom = rand() % 7;//pick a random room index
                } while(i == randRoom);   //but don't connect to self
                for(k = 2; k < 8; k ++){
                    if(connections[i][k] == randRoom)
                        k = 8;  //connection exists exit the loop, try again 
                    else if(connections[i][k] == -1){
                        //make a new connection only if the receiving room has less
                        //than or equal to 6 connections
                        if(connections[randRoom][1] <= MAXCONS){
                            //iterate through the received room list of connections
                            for(r = 2; r < 8; r++){
                                if(connections[randRoom][r] == randRoom){
                                    k = 8; //connection exists in the other list
                                    r = 8; //so exit both loops
                                }
                                else if(connections[randRoom][r] == -1){
                                    connections[i][k] = randRoom;
                                    connections[i][1] += 1;
                                    connections[randRoom][r] = i;
                                    connections[randRoom][1] += 1;
                                    k = 8; //added the connection between the two rooms
                                    r = 8; //incremented their connection count. 
                                }          //exit out of both loops
                            }
                        }
                    }
                }
            }
        }
    }

    //used for testing to print out the contents of our 2d array
/*    for(i = 0; i < 7; i++){
        for(j = 0; j < 8; j++)
            printf("%d ", connections[i][j]);
        printf("\n");
    }*/

    return connections; 
}


/****************************************
 * Function: createFiles()
 * Parameters: char* to the file directory, int** to 2d array of connections
 * Description: for each of the seven rooms create a new file in the passed
 * file directory such that the structure of the file is:
 * ROOM NAME: <room name>
 * CONNECTION #: <room name> (up to 6 possible connections)
 * ROOM TYPE: <room type>
 * Preconditions: passed a valid file directory and valid 2d array of room connections
 * Postconditions: 7 room files generated
 * *************************************/

void createFiles (char* pathName, int** connections){
    //The ten hardcoded rooms names
    //Courtesy of fantastynamegenerators.com
    char *roomNames[] = { 
                      "Labyrinth",
                      "Grotto",
                      "Haunt",
                      "Catacombs",
                      "Chambers",
                      "Vault",
                      "Delves",
                      "Marsh",
                      "Maze",
                      "Tunnels"};

    char* roomHeader = "ROOM NAME: ";
    char* connHeader = "CONNECTION ";
    char* typeHeader = "ROOM TYPE: "; 

    int j, i;
    int index;
    int startRoom = 0;  //identifies if a START_ROOM has been chosen
    int endRoom = 0;    //identifies if an END_ROOM has been chosen
    int fileDescriptor;
    char newFile[MAXCHAR];
    char buffer[100];

    srand(time(NULL)); //seed number generator so we can choose randomly the 
                       //starting and end rooms

    //for each of our seven rooms    
    for(j = 0; j < 7; j++){
        //empty the newFile to null terminated, 
        // newFile will hold ./pikelj.rooms.PID/<room name>
        // create that file and leave it open
        memset(newFile, '\0', sizeof(newFile) * sizeof(char));
        sprintf(newFile, "%s/%s", pathName, roomNames[connections[j][0]]);
        fileDescriptor = open(newFile, O_WRONLY | O_CREAT, 0664);
        
        //check that opening the file worked
        assert(fileDescriptor != -1);

        //empty the buffer used to write
        //write ROOM NAME: <room name>
        memset(buffer, '\0', sizeof(buffer) * sizeof(char));
        sprintf(buffer, "%s%s\n", roomHeader, roomNames[connections[j][0]]);
        write(fileDescriptor, buffer, strlen(buffer) * sizeof(char));

        //write connections with number then colon and connection name
        //first empty the buffer used to write
        //To get the room name that our index points to we need to
        //get the value of the connected room at connections[j][i]
        //this value at index location connections[value][0] is the 
        //index we look up in roomNames ( the hardcoded list of room names)
        //write CONNECTION #: <room name>
        //connections[j][2] is the number of connections we have for this given room
        //we need to offset this count by 2 because our connections start at index 2
        //thus we have the i < # + 2
        for (i = 2; i < connections[j][1] + 2; i++){
            memset(buffer, '\0', sizeof(buffer) * sizeof(char));
            index = connections[connections[j][i]][0];
            sprintf(buffer, "%s%d: %s\n", connHeader, i-1, roomNames[index]);
            write(fileDescriptor, buffer, strlen(buffer) * sizeof(char));
        }  
        
        //write typeHeader for room type and then the room type
        //using a simple d20 roll for each room if we haven't chosen
        //a start room yet and roll 15 or higher, then this room is the
        //START_ROOM.  Likewise once a START_ROOM is chosen and is need to
        //choose an END_ROOM if an 15 or higher is rolled, this becomes the
        //END_ROOM.  If we get the to the last two rooms without having chosen
        //a START_ROOM and END_ROOM the last two rooms become the START and END
        //rooms respectively

        memset(buffer, '\0', sizeof(buffer) * sizeof(char));
        if(startRoom == 0 && rand() % 20 + 1 >= 15){
            sprintf(buffer, "%sSTART_ROOM", typeHeader);
            startRoom = 1;
        }
        else if(endRoom == 0 && rand() % 20 + 1 >= 15){
            sprintf(buffer, "%sEND_ROOM", typeHeader);
            endRoom = 1;
        }
        else if(j == 5 && startRoom == 0){
            sprintf(buffer, "%sSTART_ROOM", typeHeader);
            startRoom = 1;
        }
        else if(j == 6 && endRoom == 0){
            sprintf(buffer, "%sEND_ROOM", typeHeader);
            endRoom = 1;
        }
        else 
            sprintf(buffer, "%sMID_ROOM", typeHeader);

        write(fileDescriptor, buffer, strlen(buffer) * sizeof(char));
        //finished with this file close it now
        close(fileDescriptor); 
    }
}
