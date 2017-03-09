/****************************************
 * Filename: pikelj.adventure.c
 * Author: Johannes Pikel
 * ONID: pikelj
 * Date: 2017.01.31
 * Class: CS344-400
 * Assignment: Program 2 adventure
 * Due: 2017.13.02
 * Description:
 * *************************************/

#include <stdio.h>      //standard input and output library
#include <fcntl.h>      //file control options
#include <stdlib.h>     //general functions library like random
#include <unistd.h>     //include for the getpid function
#include <sys/types.h>  //get file status or directory status
#include <sys/stat.h>
#include <time.h>       //so we get current time
#include <string.h>     //so we can use strcmp
#include <dirent.h>     //to retrieve the names of directories in this dir
#include <pthread.h>    //gives access to multithreading
#include <assert.h>     //check malloc's etc worked

#define MAXNAME 16      //the max length for a given filename
#define MAXPATH 64      //the max path for now for a complete filepath 
#define MAXREAD 192     //max number of bytes to read from a file (may need to come back)
#define READSTART 5     //used when we are checking for START 
#define READEND 3       //used when checking for END
#define ROWS 6          //ROWS and COLS used in the 2d options char array
#define COLS 11         //that will store the current room connections
#define DATEREAD 42     //I hope the dates are not longer than this


//threadInfo is used to store the mutex for locking and unlocking
//the ID of the thread we just created
//getTime is used by the writeTime function to see if it should write
//the current time to file when it gains access
struct threadInfo {
    pthread_mutex_t mutex;
    pthread_t thread;
    int getTime;
};    

//Some c strings that will be used with dynamic mem allocation 
//to store information about a current file that is being worked on
//fileDir does not change through the program, once set by getNewestDir
//fileName is only used in getStartFile and then released
//filePath is frequently written over for which ever file we are currently working with
struct fileInformation {
    char* fileName;
    char* filePath;
    char* fileDir;
};

typedef struct threadInfo threadInf;
typedef struct fileInformation fileInf;

//function prototypes
void getNewestDir(char*);           //get the directory that was created most recently
void getStartFile(fileInf*);    //find the file that is the START_ROOM
void printRoom(int, char[][COLS]);  //print the room contextual info
int checkVictory(int);              //check if the room is the END
char* getUserInput(threadInf *);         //get users input
int validateInput(char*, char[][COLS]); //validates if the line entered is valid
void printVictory(int);                 //print the victory statement
void playGame(int, fileInf*);              //run the game until victory
void saveRoom(char*, fileInf*);             //save the room history path
void printSavedRooms(fileInf*);       //print the room history and delete that file
void* writeTime(void*);                   //function writes the current system time
void printTime();
void handleThread(threadInf *);



/****************************************
 * Function: main()
 * Parameters: none
 * Description: creates a fileInformation struct and initializes the memory for the
 * three c-strings.
 * Locates the newest directory with the substring of pikelj.rooms.
 * Within that directory locates the file with the room type of START_ROOM
 * then plays the game starting from that room until finished.
 * Preconditions: none
 * Postconditions: heap memory freed
 * *************************************/

int main(void){
    int fileDescriptor;
    //allocate some memory for the struct and the internals of the struct
    struct fileInformation* fi = malloc(sizeof(fileInf));
    assert(fi != 0);

    fi->fileName = malloc(MAXNAME * sizeof(char));
    assert(fi->fileName != 0);

    fi->filePath = malloc(MAXPATH * sizeof(char));    
    assert(fi->filePath != 0);

    fi->fileDir = malloc(MAXPATH * sizeof(char));
    assert(fi->fileDir != 0);
    //locate the rooms directory that was created most recently
    //and store it in the struct's fileDir
    getNewestDir(fi->fileDir);

    memset(fi->filePath, '\0', MAXPATH * sizeof(char));

    //get the starting fileName and update the structs fileName
    getStartFile(fi);

    memset(fi->filePath, '\0', MAXPATH * sizeof(char));
 
    sprintf(fi->filePath, "./%s/%s", fi->fileDir, fi->fileName);

    //open the starting file and pass it to the playGame function to start
    //the game
    //through all of this the structs filePath variable is a temporary c string
    //location that we'll use to construct various file paths as new files are 
    //opened
    //I kept the fileDescriptor separate from the struct because I did not want
    //to accidently write over it in the the other functions that use our struct
    //but do not need the fileDescriptor, so keep it protected a bit by making it it's
    //own variable
    fileDescriptor = open(fi->filePath, O_RDONLY);
    assert(fileDescriptor != -1);

    //at this point in the implementation the fileName is no longer used so we can
    //go ahead and free that heap memory
    free(fi->fileName);

    //play the game starting from the START_ROOM
    playGame(fileDescriptor, fi);

    close(fileDescriptor);
    free(fi->fileDir);
    free(fi->filePath);
    free(fi);
    return(0);
}

/****************************************
 * Function: writeTime()
 * Parameters:  threadInfo struct
 * Description: this is the function that will be used in the multithread
 * it is passed in a threadInfo struct and uses that to attempt to unlock
 * the mutex. Once unlocked, if the variable getTime is set to 1, overwrite to the
 * file currentTime.txt in the current directory the current local time in the format
 * HH:MMAM/PM, DAY, MONTH, DATE, YEAR
 * Preconditions: passed in an initialized a threadInfo
 * Postconditions: time written to file 
 * Cite: http://www.cplusplus.com/reference/ctime/strftime/
 * http://stackoverflow.com/questions/14888027/mutex-lock-threads
 * *************************************/

void* writeTime(void* argument){

    //cast the passed argument to a local pointer to a threadInfo
    struct threadInfo * tInfo = (struct threadInfo *) argument;

    //block here until the main thread unlocks the mutex
    pthread_mutex_lock(&tInfo->mutex);
    //we check this variable because there will be 1 thread still waiting at the end
    //of the game and we don't want that last thread to write the time when it wasn't
    //explicitly told to do so by entering the time command
    if(tInfo->getTime == 1) {
        int fileDescriptor;
        time_t rawTime;
        struct tm * timeInfo;
    
        char* line = malloc(DATEREAD * sizeof(char));
        assert(line != 0);
        memset(line, '\0', DATEREAD * sizeof(char));
    
        //open the currentTime.txt in the current directory with ./
        //open it as write only, create if necessaru and truncate it if it exists
        fileDescriptor = open("./currentTime.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
        assert(fileDescriptor != -1);
        
        time(&rawTime); //get the raw system time
        timeInfo = localtime (&rawTime); //from that concert it to local time
        
        //write as a string HH:MM AM/PM, DAY, MONTH, DATE, YEAR
        strftime(line, DATEREAD*sizeof(char), "%I:%M%p, %A, %B %d, %Y", timeInfo);
        //write the time to the opened file and close the file
        write(fileDescriptor, line, DATEREAD*sizeof(char));
        close(fileDescriptor);
        free(line);
    }
    //allow the main thread to take back control
    pthread_mutex_unlock(&tInfo->mutex);
    return 0;
}

/****************************************
 * Function: printTime()
 * Parameters: none
 * Description: open the file in the current working directory called
 * currentTime.txt, there should only be 1 line, read that in and print to 
 * screen.
 * Preconditions: currentTime.txt exists, created by writeTime
 * Postconditions: line from file printed to screen
 * *************************************/

void printTime(){

    int fileDescriptor;

    //line will be the read buffer
    char* line = malloc(DATEREAD * sizeof(char));
    assert(line != 0);
    memset(line, '\0', DATEREAD * sizeof(char));
    //open the file, there should be no way this function gets called without
    //writeTime having been allowed to finish at least once
    fileDescriptor = open("./currentTime.txt", O_RDONLY);
    assert(fileDescriptor != -1);

    //move the pointer to the begnning of the file
    //read and print the line
    lseek(fileDescriptor, 0, SEEK_SET);
    read(fileDescriptor, line, DATEREAD * sizeof(char));
    printf("\n%s\n\n", line); 
 
    close(fileDescriptor);
    free(line);
}

/****************************************
 * Function: playGame
 * Parameters: int, struct fileInformation*
 * Description: initializes the threadInfo struct that will store the threads IDs
 * the mutex and the variable to check by writeTime
 * locks the mutes, then creates the first thread that will attemp to writeTime
 * the variable passed in is the struct threadInfo, so we transport the mutex and
 * getTime variable.
 * The function then loops through this outline:
 * print the room information (sets the options 2d array)
 * get user input (thread control handled inside this function)
 * if valid input
 *      close the current file open, 
 *      open the new file,
 *      save the previous room's name to a temporary file that will be erased
 *      increment steps taken
 * otherwise
 *      print the i don't understand comment.
 * Check if the new room entered is the END_ROOM, if so the game is won
 * print the victory message
 * print the room history path
 * unlock the mutex so the final thread can complete, getTime is set to 0 so it should
 * do nothing
 * Preconditions: passed in valid fileInformation struct and fileDescriptor
 * Postconditions: game is played until victory, heap memory freed
 * *************************************/

void playGame(int fileDescriptor, struct fileInformation* fi){
 
    int steps = 0;
    char* lineEntered;
    char options[ROWS][COLS];
    
    struct threadInfo* tInfo = malloc(sizeof(struct threadInfo));
    assert(tInfo != 0);
    tInfo->getTime = 0; //this is the variable checked by writeTime

    pthread_mutex_init(&tInfo->mutex, NULL); //initialize mutex stored in threadInfo

    pthread_mutex_lock(&tInfo->mutex); //and make sure to lock it so we have control

    //create the new thread that will try to writeTime to file, and make sure it 
    //was successful, pass in the threadInfo struct so it has access to the
    //getTime variable and the mutex
    assert(pthread_create(&tInfo->thread, NULL, writeTime, tInfo) == 0);

    do {
        memset(options, '\0', sizeof(options));
        printRoom(fileDescriptor, options);     //this prints the room information
        lineEntered = getUserInput(tInfo);      //get the users input
        if(validateInput(lineEntered, options) == 0){   
                //if a valid choice, close the currently open file
                close(fileDescriptor);
                memset(fi->filePath, '\0', MAXPATH);
                //compose the new file path here
                snprintf(fi->filePath, (strlen(fi->fileDir) + strlen(lineEntered) + 3), 
                         "./%s/%s", fi->fileDir, lineEntered);

                //open it the new file
                fileDescriptor = open(fi->filePath, O_RDONLY);
                assert(fileDescriptor != -1);
                //save the previous move to the temporary move history file
                saveRoom(lineEntered, fi);
                //we have sucessfully moved 1 step
                steps += 1;
        }
        else
            printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
        //Can now free the lineEntered this actually points to the memory location
        //create by getUserInput with the getline().
        free(lineEntered);
    } while(checkVictory(fileDescriptor) == 0);
    //continue until the room is the END_ROOM
    //print the vitory statements
    printVictory(steps);
    printSavedRooms(fi);

    //unlock the mutex 1 final time so that the other thread can expire
    pthread_mutex_unlock(&tInfo->mutex);
    pthread_join(tInfo->thread, NULL);
    //free the heap memory
    free(tInfo);
}

/****************************************
 * Function: getNewestDir
 * Parameters: char* 
 * Description: Opens the current working directory with ./
 * Checks each possible file or directory in the cwd to see if the first
 * 12 chars matach pikelj.rooms. if so then we need to compare that directories
 * st_mtime with the current newest or if newestTIme has not been set, set it now
 * copy the complete path from the newest directory into the passed char*
 * Preconditions: passed valid char*
 * Postconditions: most recently create dir with prefix pikelj.rooms. identified
 * Cite: http://stackoverflow.com/questions/12489/how-do-you-get-a-directory-listing-in-c
 * *************************************/

void getNewestDir(char* newestDir){

    char* myDir = "pikelj.rooms.";
    DIR *dirPath;
    struct dirent *entityPath;
    struct stat *fileInfo;
    time_t newestTime = 0; //difftime of this will be 0.00 

    //open the current working directory
    dirPath = opendir("./");

    //if the opendir didn't work something went wrong
    if(dirPath != NULL){
        //check all the files/directories
        while((entityPath = readdir(dirPath))){
            //there may be a better way to compare, but this seems to work for now
            if(strncmp(entityPath->d_name, myDir, 12) == 0){
                fileInfo = malloc(sizeof(struct stat));
                //stat the possible matching directory so we have access to st_mtime
                stat(entityPath->d_name, fileInfo);
                //difftime of 0, 0 will be 0 that means we have not set newestTime to
                //a directory, set it now and copy the directory name to 
                //the argument
                //then for each additional directory compare their times with difftime
                //that returns a double.  The newest file will be the one that has
                //the largest double when compared to the epoch and that is the one we
                //want to find
                if(difftime(newestTime, 0) == 0){ //first file to be checked
                    newestTime = fileInfo->st_mtime;
                    strcpy(newestDir, entityPath->d_name);
                }
                else if(difftime(newestTime, 0) < difftime(fileInfo->st_mtime, 0)){
                    newestTime = fileInfo->st_mtime;
                    strcpy(newestDir, entityPath->d_name);
                }
                free(fileInfo);
            }
        }
        closedir(dirPath);
    }
    else
        exit(1);
}

/****************************************
 * Function: getStartFile
 * Parameters: struct fileInformation*
 * Description: iterates through the file directory identified by getNewestDir.
 * Opens each file in the directory sets the file pointer to the the EOF - 10 bytes
 * reads the next 5 bytes.  If that is START then this is the START_ROOM
 * Preconditions: passed in a valid directory path
 * Postconditions: START_ROOM identified
 * *************************************/

void getStartFile(struct fileInformation* fi){

    DIR *dirPath;
    struct dirent *entityPath;
    int fileDescriptor;
    char* readBuf = malloc(READSTART * sizeof(char));

    assert(readBuf != 0);
    
    //use the filePath already in the fileInformation struct instead of reallocating local heap
    //memory.  set it to null terminated
    //then write the current directory name with prefix of ./ for current working directory
    memset(fi->filePath, '\0', MAXPATH * sizeof(char));
    sprintf(fi->filePath, "./%s", fi->fileDir);

    //open the file directory
    dirPath = opendir(fi->filePath);

    //iterate through each file in the directory
    if(dirPath != NULL){
        while((entityPath = readdir(dirPath))){
            memset(fi->filePath, '\0', MAXPATH * sizeof(char));
            sprintf(fi->filePath, "%s/%s", fi->fileDir, entityPath->d_name);

            fileDescriptor = open(fi->filePath, O_RDONLY); //readonly
            assert(fileDescriptor != -1);

            memset(readBuf, '\0', READSTART * sizeof(char));
            //from the end of each file START_ROOM is -10 bytes
            lseek(fileDescriptor, -10, SEEK_END);
            read(fileDescriptor, readBuf, READSTART * sizeof(char));
            //if the 5 bytes read match START this is the room we want to begin with
            //copy this room to the fileInformation struct fileName char*
            if(strncmp(readBuf, "START", READSTART) == 0)
                strcpy(fi->fileName, entityPath->d_name);
            close(fileDescriptor);
        }
        closedir(dirPath);
    }
    else {
        free(readBuf);
        exit(1);
    }
    //clean up heap memory
    free(readBuf);

}

/****************************************
 * Function: printRoom()
 * Parameters: int, 2d array of char*[][]
 * Description: Using the passed in fileDescriptor int, that should point to an open file
 * set the file pointer to the beginning of the file and then 11 bytes into the file.  
 * This should but immediately after the first colon after ROOM NAME:.  Read 
 * from the file the MAXREAD bytes which is 192.  At the present time even with all the
 * longest room names and 6 connections, this should be enough to read what I calculated
 * to be 174 bytes for a file.
 * Then print the bytes read until we run into a \newline.  This is the end of the room
 * name.
 * Print the connections
 * Preconditions: valid int fildescriptor, populated 2d array of char 
 * Postconditions: Room information printed to screen
 * *************************************/

void printRoom(int fileDescriptor, char (*options)[COLS]){

//    int curPos;
    int i, j, k, done;

    char* readBuf = malloc(MAXREAD * sizeof(char));
    assert(readBuf != 0);

    lseek(fileDescriptor, 0, SEEK_SET); //reset pointer to beginning of file
    lseek(fileDescriptor, 11, SEEK_SET);  //move pointer 11 characters in
                                            //should be just after the first colon
    printf("\nCURRENT LOCATION: ");
    memset(readBuf, '\0', MAXREAD);
    read(fileDescriptor, readBuf, MAXREAD*sizeof(char));
    //at the pointer we should have the entire rest of the file read into the
    //buffer
    //after the seek above when we reach the newline that is the end of room name.
    for(i = 0; i < strlen(readBuf) && readBuf[i] != '\n'; i++)
        printf("%c", readBuf[i]);
    
    printf("\nPOSSIBLE CONNECTIONS: ");
    //print the room connections, carry on with i where it left off above
    //never reset i to 0 so we continue to move through the file
    done = 0;
    j = 0;
    while(!done){
        k = 0;
        //iterate through the read buffer until we run into a colon
        //do nothing else, just increment i
        for(; i < strlen(readBuf) && readBuf[i] != ':'; i++){}
        //if the byte before is not a 1 and not an E we need to print a comma
        //because this is an additional connection
        if(readBuf[i-1] != '1' && readBuf[i-1] != 'E')
            printf(", ");
        //if the first if passed meaning we have a 1 then only print the
        //name of the conection until we get to the newline
        if(readBuf[i-1] != 'E'){
            i = i + 2;  //skip the space after the colon
                        //and now print the word until the newline is encountered
            for(; i < strlen(readBuf) && readBuf[i] != '\n'; i++){
                //at the same time we want to populate the options 2d array of
                //c strings such that each row contains 1 complete name
                //so we only increment j after printing a complete room name
                //while k increments with each char
                options[j][k++] = readBuf[i];
                printf("%c", readBuf[i]);    
            }
            //add a newline to the end of the this row, so we can easily
            //strcmp it to the lineEntered by user which will also have a
            //newline char at the end
            strcat(options[j], "\n");
            j++;
        }
        //if the previous byte is an E then we are at the end print a period
        if(readBuf[i-1] == 'E'){
            printf(".\n");
            done = 1;
        }
    }

    free(readBuf);
}

/****************************************
 * Function: checkVictory
 * Parameters: int fileDescriptor
 * Description: given a valid file descriptor of an open file, 
 * seek -8 bytes from the end of the file.  Read 3 bytes there
 * those 3 bytes should line up with the END if that is a match then
 * return 1, signaling that we have reached the end! Otherwise return 0
 * Precondition: valid file descriptor of an open file
 * Postcondition: return int
 * *************************************/

int checkVictory(int fileDescriptor) {

    char* readBuf = malloc(READEND * sizeof(char));

    assert(readBuf != 0);

    memset(readBuf, '\0', READEND);
    //for all files as currently constructed -8 bytes from the end
    //should be END if it does exist.
    lseek(fileDescriptor, -8, SEEK_END);
    read(fileDescriptor, readBuf, READEND);

    if(strncmp(readBuf, "END", READEND) == 0){
        free(readBuf);
        return(1);
    }
    else {
        free(readBuf);
        return(0);
    }
}

/****************************************
 * Function: getUserInput()
 * Parameters: struct threadInfo*
 * Description: print the line WHERE TO? >
 * Then get the line entered from stdin ( keyboard )
 * if that line is time with a newline then we need to jump over
 * to handleThread().  Only return the lineEntered if it is not
 * "time"
 * Precondition: valid initializes threadInfo with a thread and mutex
 * Postcondition: return the char* 
 * *************************************/
char* getUserInput(struct threadInfo* tInfo){
    char* lineEntered = NULL; 
    size_t bufSize = 0;
    int charsRead = -5;
    int done = 0;

    do {
        printf("WHERE TO? >");

        charsRead = getline(&lineEntered, &bufSize, stdin);
        if(strcmp(lineEntered, "time\n") == 0){
            free(lineEntered);
            lineEntered = NULL;
            handleThread(tInfo);
        }
        else {
            done = 1;
        }
    } while (done == 0);
    
    return(lineEntered);
}
/****************************************
 * Function: handleThread()
 * Parameters: struct threadInfo*
 * Description: sets the getTIme variable to 1 so that our other thread
 * will write the time to file. 
 * Unlock the mutex to allow the other thread access and then wait for it
 * to complete it with join.  Once it has finished lock the mutex again
 * and set the getTime variable to 0.  print the time written in the file.
 * Create another thread so that if the user enters "time" again it will
 * be updated in the currentTime.txt
 * setting getTime to 0 and finishing the game will prevent the thread
 * from writting to the file when not requested.
 * Precondition: valid thread and mutex in struct threadInfo
 * Postcondition: thread allowed to complete, then create another thread.
 * *************************************/
void handleThread(struct threadInfo* tInfo){

    tInfo->getTime = 1;
    pthread_mutex_unlock(&tInfo->mutex);
    pthread_join(tInfo->thread, NULL); 
    pthread_mutex_lock(&tInfo->mutex);
    tInfo->getTime = 0;
    printTime();
    assert(pthread_create(&tInfo->thread, NULL, writeTime, tInfo) == 0);

}

/****************************************
 * Function: validateInput()
 * Parameters:char*, char* [][]
 * Description:Passed a valid c_string with lineEntered will iterate 
 * through the 2d array of c-strings in options[][].  Each row is a 
 * room name, that was populated in printRoom().  If we find one that
 * matches return 0
 * Precondition: valid lineEntered string, and populated 2d char array
 * Postcondition: returns int, 0 for true.
 * *************************************/

int validateInput(char* lineEntered, char (*options)[COLS]){
    int i = 0;
    while(i < ROWS && *options[i]) {
        if(strcmp(options[i++], lineEntered) == 0)
            return(0);
    }

    return(1);
} 


/****************************************
 * Function: printVictory()
 * Parameters: int
 * Description: prints two lines of text and the
 * number of steps taken. for the victory message
 * Precondition: int passed in
 * Postcondition: message printed to screen
 * *************************************/

void printVictory(int steps){
    printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
}

/****************************************
 * Function: saveRoom()
 * Parameters: char*, struct fileInformation*
 * Description: Creates a temporary file in the same directory as 
 * the current rooms being explored called savedRooms.PID
 * Opens this file and appends to it, by seeking to the end of the file.
 * I was having some trouble with the O_APPEND (need to investigate).  
 * Then at the end of the file write the lineEntered, which at this point
 * is a valid room name. Each lineEntered already has a newline on it
 * so no need to add one here.
 * Precondition: passed char*, fileDir in fileInformation* is the room
 * file directory
 * Postcondition: room written to file
 * *************************************/

void saveRoom(char* lineEntered, struct fileInformation* fi){
    int fileDescriptor;
    pid_t pid = getpid();
    
    memset(fi->filePath, '\0', sizeof(fi->filePath) * sizeof(char));

    sprintf(fi->filePath, "./%s/savedRooms.%d", fi->fileDir, (int) pid);

    fileDescriptor = open(fi->filePath, O_WRONLY | O_CREAT, 0664);

    lseek(fileDescriptor, 0, SEEK_END);

    write(fileDescriptor, lineEntered, strlen(lineEntered));

    close(fileDescriptor);
}

/****************************************
 * Function: printSavedRooms()
 * Parameters: struct fileInformation*
 * Description: using the file directory in the fileInformation struct
 * opens the savedRooms.PID file created while playing the game.
 * Sets the file poitner to the beginning of the file with lseek.
 * Reads the entire file one char at a time until the end.  Because
 * the file was created with newlines, it will print out a vertical list
 * of the rooms in order until the end of the file.
 * Precondition: valid fileInformation struct
 * Postcondition: rooms printed to screen
 * *************************************/

void printSavedRooms(struct fileInformation* fi){
    int fileDescriptor;
    char readBuf;
    pid_t pid = getpid();

    memset(fi->filePath, '\0', sizeof(fi->filePath) * sizeof(char));

    sprintf(fi->filePath, "./%s/savedRooms.%d", fi->fileDir, (int) pid);

    fileDescriptor = open(fi->filePath, O_RDONLY);

    lseek(fileDescriptor, 0, SEEK_SET);
    //continue reading 1 char until read returns 0 at which point
    //we are at the EOF
    while(read(fileDescriptor, &readBuf, sizeof(char)) > 0)
        printf("%c", readBuf);

    close(fileDescriptor);
    //close the file and make sure to remove this file as it is only
    //temporary
    remove(fi->filePath);
}
