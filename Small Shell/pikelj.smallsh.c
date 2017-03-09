/****************************************
 * Filename: pikelj.smallsh.c
 * Author: Johannes Pikel
 * Date: 2017.02.14
 * ONID: pikelj
 * Class: CS344-400
 * Assignment: Program 3 smallsh
 * Description:
 * *************************************/

#include <stdio.h>      /* standard input and output */
#include <stdlib.h>     /* standard library function */
#include <unistd.h>     /* for read() and write() */
#include <sys/wait.h>   /* so we can use waitpid */
#include <assert.h>     /* assert for testing cases */
#include <string.h>
#include <sys/types.h>  /* for unix system types */
#include <fcntl.h>      /* file control */
#include <signal.h>     /* allow use of sigaction SIGCHLD */
#include <sys/mman.h>

/* Constants are defined here */
#define MAXLEN 2050     /* So we can read in 2048 characters plus the \n and \0 */
#define MAXARG  512     /* So we may have a max of 512 arguments from cmdline */
#define MAXFILE 64      /* arbitrary maximum filename used in cd */
#define MAXPIDS 256     /* max number of background PIDS, change as needed */

#define DELIMS " \n"    /* used in the parsePrompt function by strtok */

typedef struct {        /* a struct to pass a bunch of info around to various */
    int doInFile;       /* functions, so we can just pass one variable around */
    int doOutFile;      /* and get access to all this information */
    int background;
    char inFile[MAXFILE];
    char outFile[MAXFILE];
    pid_t forkPID;
    int childExit;
    int done;
    char* myPID;
} cmdInfo;

/*the next two are used in the sigaction handlers, only changed between 0 or 1 */
volatile sig_atomic_t childKilled = 0;
volatile sig_atomic_t allowBackground = 1;
/* the two signales we'll be catching with sigaction, initialized in initHandler() */ 
struct sigaction interruptAct, suspendAct, childSIGTSTP;
/* Function Prototypes */
void prompt();                          /* prompt to stdout */
char* readPrompt();                     /* read user input */
cmdInfo* parsePrompt(char*, char**);    /* parse user input */
cmdInfo* initCmdInfo();                 /* initialize the cmdInfo struct */
int openOutFile(char*);                 /* create output file, redirect stdout */
int openInFile(char*);                  /* change stdin to point to file */
void launchProcess(char**, cmdInfo*);   /* launch child process */
int executeCmd(char**, cmdInfo*, int);  /* decider function of how to handle input */
void checkExit(int);                    /* check exit status of process */
void flushOut();                        /* flushes stdout */
void initSigHandlers();                 /* initialize the signal handlers */
int checkFGChildDone(cmdInfo*, int);    /* check if foreground process is done */
void checkBGChildDone(int*);            /* check if background process is done */
void cleanUpBGChildren(int*);
void changeDir(char**);                 /* shell built-in to change directory */
int* initPIDS();                        /* initialize the background PIDS array */
void addPID(cmdInfo*, int*);            /* add background PID to storage */
char* searchPID(char*);                 /* function to search and replace PID or $$ */
void interruptHandler(int);             /* handler for SIGTSTP or CTRL-Z */
void suspendHandler(int);               /* handler for SIGTERM or CTRL-C */
/*void childHandler(int);*//* no longer in USE */

/****************************************
 * Function: main()
 * Parameters: none
 * Description: runs a continuous loop of the shell until the exit command is entered
 * then the done variable is set to -1 and the loop exists.  As the shell processes
 * it should properly handle allocation and freeing heap memory
 * Preconditions: none 
 * Postconditions: shell runs and heap memory freed
 * *************************************/

int main(void) {

    char* lineEntered;
    char* args[MAXARG + 1];         /* +1 for the NULL at the end of array*/
    int done = 0;
    cmdInfo* info;
    int lastFGChild = -5;           /* stores the last foreground child PID */
    int* pids;

    pids = initPIDS();              /* a storage array of the background PIDS */
        
    initSigHandlers(); 
                                        /* initialize the signal handlers, done once
                                        sigaction remains after a signal is caught
                                        unlike signal() which requires reactivation */

    do{
        lineEntered = readPrompt();             /* read the user prompt */
        lineEntered = searchPID(lineEntered);   /* search input for $$ or PID value */
        info = parsePrompt(lineEntered, args);  /* parse prompt into args array */

        done = executeCmd(args, info, lastFGChild); /* returns exit value */
        addPID(info, pids);                         /* see if process is in background*/
                                                    /* store bg PID in pids array */
                                                /* wait until foreground process is */
                                                /* and store it's PID for future use */
        lastFGChild = checkFGChildDone(info, lastFGChild);
        checkBGChildDone(pids);                 /* check if any background processes */
                                                /* are done */

        if(info->myPID != NULL)                 /* free some heap memory */
            free(info->myPID);                  /* could probably change the alloc */
        free(info);                             /* to main here, and only free once */
                                                /* but this way ensures we don't have */
        free(lineEntered);                      /* stale data */
    } while(done == 0);
    
    cleanUpBGChildren(pids);
    free(pids);
    return(0);
}

/****************************************
 * Function: prompt()
 * Parameters: none
 * Description: writes a colon and space to stdout, normally should be the screen
 * unless stdout is redirected to a file for saving
 * Preconditions: none
 * Postconditions: 2 bytes written to stdout
 * *************************************/

void prompt() {
    write(STDOUT_FILENO, ": ", 2);
}

/****************************************
 * Function: readPrompt()
 * Parameters: none
 * Description: allocates some heap memory for a buffer that will read in from
 * STDIN.  The buffer is the size of MAXLEN as defined above.  Uses fgets to read
 * to the end of the line.  If we receive an error back from the system call that 
 * fgets uses, we'll clear the buffer, write a newline and write the prompt again,
 * then fgets should restart automatically.  We may receive bad input when one of the
 * signal handlers catches SIGINT, SIGTSTP or the like
 * Preconditions: heap memory available
 * Postconditions: returns pointer to the char string location on the heap
 * *************************************/

char* readPrompt() {
    char* buffer = NULL;
/*    size_t bufSize = 0;
    int charsRead = -5;
    int done = 0;*/
    char* ret;
    buffer = malloc(MAXLEN * sizeof(char));
    assert(buffer != 0);
    memset(buffer, '\0', MAXLEN * sizeof(char));

    prompt();

    /* if we receive any sort of error fgets returns NULL, clear buffer, and try again*/
    while((ret = fgets(buffer, MAXLEN,stdin)) == NULL){
        fflush(stdin);
        memset(buffer, '\0', MAXLEN * sizeof(char));
        write(STDOUT_FILENO, "\n", 1);
        prompt();
    }
    
    return buffer;
}

/****************************************
 * Function: parsePrompt()
 * Parameters: char*, char**
 * Description: Parses the line entered by the user, received by the readPrompt function
 * Each token in the string that is separated by a space is added as a pointer to that 
 * location to the args array.  The args array is an array of pointers to these c string
 * locations, so that execvp works correctly.
 * If we encounter special characters such as < > or $$ we need to handle those.
 * At the end we iterate through the args array to the last pointer before the end 
 * designated by NULL, if this pointer containes the & we know this program should 
 * run in the background unless, the first arg[0] is status, in that case we ignore it
 * regardless if we find an & at the end of a statement we need to overwrite that pointer
 * as NULL otherwise execvp will not understand what to do.
 * Preconditions: passed in pointer to a line of user input and a valid pointer to an
 * array of char*
 * Postconditions: line is parsed into appropriate args for use in execvp
 *                  returns the struct cmdInfo as populated by this function
 * Reference: 
 * http://stackoverflow.com/questions/15539708/passing-an-array-to-execvp-from-the-users-input
 * *************************************/

cmdInfo* parsePrompt(char* lineEntered, char** args) {

    cmdInfo* info;
    char** next = args; /* we can use pointer addition to iterate through args array */
    char* temp, *saveptr;

    memset(args, '\0', sizeof(args)*sizeof(char));
    info = initCmdInfo();                           /* initialize the info struct */
    /* start our strtok here DELIMS are " \n" */
    temp = strtok_r(lineEntered, DELIMS, &saveptr); 


    while(temp != NULL){
        /* need to redirect input save in the info struct that we should process an */
        /* input file, and skip this token i.e. do not add it to args */
        /* copy the file name to our info struct for later use */
        /* the file will not be added to args, because we have another call to */
        /* strtok below */
        if(strcmp(temp, "<") == 0){
            info->doInFile = 1;
            temp = strtok_r(NULL, DELIMS, &saveptr);
            strcpy(info->inFile, temp);
        }
        /* need to redirect output, similar to above save the fact we have an output */
        /* file and save it's name to our struct for future use */
        else if(strcmp(temp, ">") == 0){
            info->doOutFile = 1;
            temp = strtok_r(NULL, DELIMS, &saveptr);
            strcpy(info->outFile, temp);
        }
        /* this word is the $$ or PID, if we come across a lone $$ in our line  */
        /* expand that $$ to the PID of the shell, store the PID in the info struct */
        /* point our arg to this location and skip the $$ in the args */
        else if(strcmp(temp, "$$") == 0){
            info->myPID = malloc(sizeof(int)*sizeof(char));
            memset(info->myPID, '\0', sizeof(info->myPID)*sizeof(char));
            sprintf(info->myPID, "%d", (int)getpid());
            *next++ = info->myPID;
        }
        /* if none of the above apply, this token should be something to add to our */
        /* args array */
        else {
            *next++ = temp;
        }
        /* move to the next token in the line */
        temp = strtok_r(NULL, DELIMS, &saveptr);
    }
    /* make the last pointer be NULL */
    *next = NULL;

    /* iterate through our args array until we reach the end, then back up one */
    /* if the last char is & and we are in the background allowed mode, store */
    /* that we want to run this process as a background process */
    /* replace the & with NULL, so that execvp doesn't fail */
    /* regardless change & to NULL even if allowBackground is not allowed */
    for(next = args; *next != 0; next++){}
    --next;
    if(args[0] != NULL && strcmp(*next, "&") == 0 && allowBackground == 1){
        info->background = 1;
    } 
    /* did some overtly verbose statements here to make sure we kill the */
    /* ampersands, :) */
    if(args[0] != NULL && strcmp(*next, "&") == 0 && allowBackground == 0){
        info->background = 0; 
    }
    if(args[0] != NULL && *next != NULL && strcmp(*next, "&") == 0){
        *next = NULL;
    }

    /* if our first arg is status enforce the rule to ignore & */
    if(args[0] != NULL && strcmp(args[0], "status") == 0){
        info->background = 0;
    }

    
    return info;
}

/****************************************
 * Function: initCmdInfo()
 * Parameters: none
 * Description: allocates heap memory for a cmdInfo struct,
 * Initializes all the parts of the struct to default values
 * Preconditions: heap memory avaialble
 * Postconditions: returns pointer to the struct cmdInfo
 * *************************************/

cmdInfo* initCmdInfo() {
    cmdInfo* info;

    info = malloc(sizeof(cmdInfo));
    assert(info != 0);              /* if malloc failed stop! */
    info->doInFile = 0;             /* 0 means do not do this */
    info->doOutFile = 0;
    info->background = 0;
    memset(info->inFile, '\0', MAXFILE*sizeof(char));   /* write null to the c strings*/
    memset(info->outFile, '\0', MAXFILE*sizeof(char));
    info->forkPID = -5;             /* store some impossible PID here */
    info->childExit = -5;
 /*   info->done = 0;*/             /* delete ? */
    info->myPID = NULL;             /* storage location for shell's PID */

    return info;   

}

/****************************************
 * Function: openOutFile()
 * Parameters: char*
 * Description: when passed a c string containing a file name, opens that file,
 * creates it if it doesn't exist, if it does exist truncates it, and in either
 * case opens it for write only.  
 * Preconditions: passed a valid file name
 * Postconditions: file created and stdout points to this file
 * *************************************/

int openOutFile(char* file) {
    int fd;
 /*   printf("file:%s\n", file);*/
    /* open the file for writeing only, create if not exists, and truncate */
    fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd == -1 ) {
        perror("open()");
        return(1);
    }
    /* set a file control so that it closes this file on exec */
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    /* make it so that STDOUT points to this file descriptor */
    if(dup2(fd, STDOUT_FILENO) == -1){
        perror("dup2");
        return(1); 
    }

    return(0);

}

/****************************************
 * Function: openInFile()
 * Parameters: char* 
 * Description: attempts to open a file for read only, prints error if it does not exist
 * also changes the stdin to read in from the file with dup2
 * Preconditions: passed in valid file name
 * Postconditions: file opened and stdin points to this file
 * *************************************/

int openInFile(char* file) {
    int fd;
    fd = open(file, O_RDONLY);
    if(fd == -1) {
        printf("cannot open %s for input\n", file);
        flushOut();
        return(1);
    }
    /* on exec close this file */ /* only stdin points to this file now */
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    if(dup2(fd, STDIN_FILENO) == -1){
        perror("dup2");
        return(1);
    }
    
    return(0);

}

/****************************************
 * Function: launchProcess()
 * Parameters: char**, cmdInfo*
 * Description: checks if we have an in file or out file requrement.  If so call
 * those functions to create either file and redirect stdin and stdout as needed.
 * If either of these function fail, we need to abor this process violently for now
 * I'm sure there should be a better method.  But this seems to work for the moment.
 * Then with the passed in args array of pointers to the correct args we call execvp()
 * so that any system built-in is searched for in the PATH variable.  If execvp fails
 * print error message and exit (1).  This should not cause the shell to exit.
 * Preconditions: passed in valid char* array of pointers to the appropriate args
 *                  cmdInfo struct with valid information
 * Postconditions: files are opened/created as required, command executed
 * *************************************/

void launchProcess(char** args, cmdInfo* info){
    int devNullIn;
    int devNullOut;
    /* if we have an input file open that file here */
    if(info->doInFile == 1){
        if(openInFile(info->inFile) == 1){
            free(info);
            exit(1);
        }
    }/* if we do not and the process is meant to run in the background */
        /* redirect STDIN to /dev/null */
    else if(info->background == 1 && info->doInFile == 0){
        devNullIn = open("/dev/null", O_RDONLY);
        fcntl(devNullIn, F_SETFD, FD_CLOEXEC);
        if(dup2(devNullIn, STDIN_FILENO) == -1){
            perror("dup2");
            exit(1);
        }
    }
 
    /* similaryly if we have an out file redirect STDOUT to that */
    if(info->doOutFile == 1){
        if(openOutFile(info->outFile) == 1){
            free(info);
            exit(1);
        }
    }
    /* otherwise if we run in background redirect outfile to /dev/null */
    else if(info->background == 1 && info->doOutFile == 0){
        devNullOut = open("/dev/null", O_WRONLY);
        fcntl(devNullOut, F_SETFD, FD_CLOEXEC);
        if(dup2(devNullOut, STDOUT_FILENO) == -1){
            perror("dup2");
            exit(1);
        }
    }
    
    execvp(args[0], &args[0]);
    printf("%s: no such file or directory\n", args[0]);
    flushOut();
    exit(1);

}

/****************************************
 * Function:executeCmd()
 * Parameters: char**, cmdInfo*, int
 * Description: Using the passed in char* array, checks to see if we have received
 * nothing or a # in the first argument, If so we've received an empty line or a comment
 * write a newline to STDOUT and go back to reading in user input.
 * If we received "exit" we want to exit the shell.  
 * If we have status and we've run at least one other foreground process so that
 * the variable lastFGChild does not contain -5 we can get the exit status of the
 * last foreground process
 * If args[0] contains cd we process this request manually as well
 * Otherwise, we create a fork and then launch the process
 * Preconditions: passed valid char* array, cmdInfo struct, and integer
 * Postconditions: command is executed properly.
 * *************************************/
int executeCmd(char** args, cmdInfo* info, int lastFGChild){

    /* NULL means our parsed encountered only /n */
    /* or encounter comment, write a newline and carry on */
    if(args[0] == NULL || strncmp(args[0] , "#", 1) == 0
        || strcmp(args[0], "\t") == 0 || strncmp(args[0], " ", 1) == 0 ){
        write(STDOUT_FILENO, "\n", 1);
    }
    /* returns -1 so that main knows to exit the shell */
    else if(strcmp(args[0], "exit") == 0){
        return(-1);
    }
    /* if we have status and we've run at least one process check that */
    /* previous process' exit status */
    else if((strcmp(args[0], "status") == 0) && lastFGChild != -5){
        checkExit(lastFGChild);
    }
    /* handle any calls for change directory here */
    else if((strcmp(args[0], "cd") == 0)){
        changeDir(args);
    }
    /* otherwise attempt to fork and create a new process here */
    else {

        info->forkPID = fork();
        if(info->forkPID == -1){
            perror("Hull Breach!");
            exit(1);
        }
        else if (info->forkPID == 0){
            /* if our process is a background process set a new signal */
            /* that will ignore any SIGINT */
            if(info->background == 1){
                signal(SIGINT, SIG_IGN);
            }
            /* we want to ignore the SIGTSTP for any child process */
            sigaction(SIGTSTP, &childSIGTSTP, NULL);

            launchProcess(args, info);
        }
    }
    return(0);
}

/****************************************
 * Function:checkExit()
 * Parameters: int
 * Description: using the passed in childExitmethod value first checks if the 
 * child exited successfully if so writes a line about the method used to exit
 * Otherwise checks if the child exited due to a signal and then writes to 
 * STDOUT the signal that caused the child to terminate.
 * Preconditions: passed in valid exit method of a process
 * Postconditions: writes to screen the exit value or signal value
 * *************************************/

void checkExit(int childExit) {

    if(WIFEXITED(childExit)){
        printf("exit value %d\n", WEXITSTATUS(childExit));
        flushOut();
    }
    else if(WIFSIGNALED(childExit)){
        printf("terminated by signal %d\n", WTERMSIG(childExit));
        flushOut();
    }
}

/****************************************
 * Function: flushOut()
 * Parameters: none
 * Description: utlity function to call fflush(stdout)
 * Preconditions: none
 * Postconditions: stdout if flushed
 * *************************************/

void flushOut(){
   fflush(stdout); 
}

/****************************************
 * Function: initSigHandlers()
 * Parameters: none
 * Description: intializes three signal handlers for the shell 
 * SIGTSTP, SIGINT
 * each is handled by their respective handler functions.
 * sigaction can be called once for intialization and then remains active
 * as long as our shell is active.
 * Preconditions: none
 * Postconditions: signal handlers are active
 * *************************************/

void initSigHandlers() {

    /* handles the SIGINT or CTRL-C signal */
    interruptAct.sa_handler = interruptHandler;
    interruptAct.sa_flags = 0;
    sigfillset(&interruptAct.sa_mask);

    /* handles the SIGTSTP or CTRL-Z signal */
    suspendAct.sa_handler = suspendHandler;
    suspendAct.sa_flags = SA_RESTART;
    sigfillset(&suspendAct.sa_mask);

    /* this is for the child process to ignore the supend SIGTSTP*/
    childSIGTSTP.sa_handler = SIG_IGN;
    childSIGTSTP.sa_flags = SA_RESTART;
    sigfillset(&childSIGTSTP.sa_mask);

    /*the childHandler is not longer in use! */
/*    childAct.sa_handler = childHandler;
    childAct.sa_flags = 0;
    sigfillset(&childAct.sa_mask);*/

    sigaction(SIGINT, &interruptAct, NULL);
    sigaction(SIGTSTP, &suspendAct, NULL);
    /*sigaction(SIGCHLD, &childAct, NULL);*/
}

/****************************************
 * Function: checkFGChildDone()
 * Parameters: cmdInfo*, int
 * Description: if the child was created as a foreground process from the parser
 * this information is stored in the cmdInfo struct and we have a valid forkPID that
 * is not -5, then we have a foreground process. We need to wait and hold until this
 * process finishes.  If the child was terminated while we were waiting, as received
 * by the childKilled variable, we need to check that childs termination status
 * If the process is meant to run in the background then we print the child PID.
 * returns the lastFGChild from waitpid to the caller.  If has changed from the waitpid
 * function otherwise just sends the same value back
 * Preconditions: passed in cmdInfo pointer and integer lastFGChild
 * Postconditions: returns int
 * *************************************/
int checkFGChildDone(cmdInfo* info, int lastFGChild){
    /* the child is a foreground process and an active process if forkPID is not -5 */
    if(info->background == 0 && info->forkPID != -5){
        /* wait until it is done */
        waitpid(info->forkPID, &lastFGChild, 0);
        if(childKilled == 1){ /* if it was terminated check its exit status */
            childKilled = 0;
            waitpid(info->forkPID, &lastFGChild, 0);
            checkExit(lastFGChild);
        }
    }
    /* otherwise if it should run the background, only prints it's PID to stdout */
    else if(info->background == 1){
        printf("background pid is %d\n", info->forkPID);
        flushOut();
    }
    
    return lastFGChild;
}

/****************************************
 * Function: checkBGChildDone()
 * Parameters: int*
 * Description: using the passed in pointer to an int array of PIDS, 
 * if we are allowing background processes
 * iterate through the entire array of stored background PIDS, if any are not -5
 * check to see if this particular PID has finished with WNOHANG so we don't hang.
 * if this particular one has finished, we'll replace this PID's value in the array with
 * -5 and then print it's exit status
 * Preconditions: passed in pointer to array of int*
 * Postconditions: if a background child has finished print that background child's
 * exit status
 * *************************************/

void checkBGChildDone(int* pids){

    int bgChild, status, i;

    /* if a background process was created and then the fore-ground only mode */
    /* is entered this will clear those background processes once background mode */
    /* is allowed again */
    /* if background processes are allowed check for them to be finished */
    if(allowBackground == 1){
        /* the entire PIDS array is initialized to -5, no PID can be -5 */
        /* only check against those PIDS that are not -5 */
        for(i = 0 ; i < MAXPIDS;i++){
            if(pids[i] != -5){
                if((bgChild = waitpid(pids[i], &status, WNOHANG)) > 0){
                    printf("background pid %d is done: ", bgChild);
                    flushOut();
                    checkExit(status);
                    pids[i] = -5;
                }
            }
        }
    }
}
/****************************************
 * Function: cleanUpBGChildren
 * Parameters: int*
 * Description: Iterates through the array of ints that should be either a -5 or a 
 * valid PID for a child.  If the loop sees any that are not -5 then it kills that
 * child.  Used to clean up any child processes on exit of the shell
 * Precondition: given a valid pointer to an array of ints
 * Postcondition: any active background children are killed
 * **************************************/
void cleanUpBGChildren(int* pids){
    int i;
    
    for(i = 0; i < MAXPIDS; i++){
        if(pids[i] != -5){
            kill(pids[i], SIGKILL);
        }
    }
}
/****************************************
 * Function: changeDir()
 * Parameters: char**
 * Description: Changes the directory to either the path stored in the environment
 * variable HOME if only "cd" is passed in
 * otherwise attempts to the change the directory to what ever args[1] points to
 * the args[0] contained "cd" was checked previously. 
 * Preconditions: passed in valid char** args array
 * Postconditions: directory changed sucessfully if it exists
 * *************************************/

void changeDir(char** args){
    /* if args[1] is null then only "cd" passed in change to HOME dir */
    if(args[1] == NULL){
        if(chdir(getenv("HOME")) != 0)
            perror("dsh");
    }
    /* otherwise change to the dir in args[1] */
    else {
        if(chdir(args[1]) != 0)
            perror("dsh");
    }
}

/****************************************
 * Function: initPIDS()
 * Parameters: none
 * Description: allocates heap memory for an array of int to the size stored in MAXPIDS
 * intializes the contents of the array to -5.  A valid PID in the system can never
 * be -5
 * return the pointer to this memory location
 * it should be noted pids array is only used to store background process PIDS
 * Preconditions: none
 * Postconditions: returns pointer to int array
 * *************************************/

int* initPIDS(){

    int i;
    int *pids;

    pids = malloc(MAXPIDS * sizeof(int));

    assert(pids != 0);

    for(i = 0; i < MAXPIDS; i++){
        pids[i] = -5;
    }

    return pids;
}

/****************************************
 * Function: addPID()
 * Parameters: cmdInfo*, int*
 * Description: if the process is determined to be a background process, iterate
 * through the PIDS array.  If happen to find same PID already exists which is unlikely
 * because two processes may not have the same PID and when a process exists we overwrite
 * it's location in the array with -5 in checkBGChildDone, this should never happen
 * but here just in case. Otherwise the first -5 we encounter we store the PID in the
 * array and then exit searching.
 * Preconditions: passed in valid cmdInfo* and int array
 * Postconditions: if the process is a background process store it's PID in the array
 * *************************************/
void addPID(cmdInfo* info, int* pids){

    int i;

    if(info->background == 1){
        for(i = 0; i < MAXPIDS; i++){
            if(pids[i] == info->forkPID)
                i = MAXPIDS;
            else if(pids[i] == -5){
                pids[i] = info->forkPID;
                i = MAXPIDS;
            }   
        }
    }
}

/****************************************
 * Function: searchPID()
 * Parameters: char*
 * Description: This function searches through the user input for either the
 * value of the parent PID or "$$" if it finds either value in the user input
 * replaces this value with the shell's own PID. returns the modified char string
 * Handles it's own memory allocation and free.  I assume a PID will not be more than
 * 20 bytes long.  
 * Preconditions: passed a pointer to a c string
 * Postconditions: returns a pointer to a c string
 * *************************************/

char* searchPID(char* lineEntered){
    int pos;
    char* pid;
    char* result;

    /* allocated some space for our shell's PID */
    pid = malloc(20*sizeof(char));
    memset(pid, '\0', sizeof(int)*sizeof(char));

    /* cast out shell's parent PID to an INT and then write that into our c string */
    sprintf(pid, "%d", (int)getppid());
    /* search the passed user input for the parent PID */
    result = strstr(lineEntered, pid);

    /* if this returns NULL search again for a substring containing $$ */
    if(result == NULL)
        result = strstr(lineEntered, "$$");

    /* if either of the above resulted in a valid substring we need to replace */
    /* that substring with our shell's PID */
    if(result != NULL){
        pos = result - lineEntered;
        for( ; pos < strlen(lineEntered); pos++){
            lineEntered[pos] = '\0';
        }

        memset(pid, '\0', sizeof(int)*sizeof(char));

        sprintf(pid, "%d", (int)getpid());

        strcat(lineEntered, pid);
    }

    free(pid);

    return lineEntered;
    
}

/****************************************
 * Function: interruptHandler()
 * Parameters: int
 * Description: when a child process receives the interrupt this command catches that
 * signal, effectively keeping the shell from exiting and the childKilled variable is
 * set to 1.  The main process or shell will then check for this.  Only works for
 * foreground processes
 * Preconditions: foreground process is set
 * Postconditions: updates childKilled variable
 * *************************************/

void interruptHandler(int sig) {
    childKilled = 1;
}

/****************************************
 * Function: suspendHandler()
 * Parameters: int
 * Description: Checks the status of the allowBackground variable and swaps it when
 * the SGTSTP or CTRL-Z is triggered.  Also prints the correct message to STDOUT to 
 * inform the user of the change in the shells behaviour.  This variable is checked in
 * parsePrompt and also in the checkBGCHildDone function
 * Preconditions: sigaction called for this signal
 * Postconditions: change in behaviour of shell
 * *************************************/

void suspendHandler(int sig) {

    if(allowBackground == 1){
        allowBackground = 0;
        write(STDOUT_FILENO, "Entering foreground-only mode (& is now ignored)\n", 49);
    }
    else if(allowBackground == 0){
        allowBackground = 1;
        write(STDOUT_FILENO, "Exiting foreground-only mode\n", 29);
    }
    /* flush output */
    fflush(stdout);
    /* not sure why, when I run through the example input from the project page */
    /* when I enter foreground-only mode after the date is called I get it's */
    /* exit status as if it was killed but it was not */
    /* not sure if my input is getting passed in in some fashion? */
    childKilled = 0;
}

/*This function is not used, only here for testing purposes*/
/*
void childHandler(int sig){
    write(STDOUT_FILENO, "Child_Done", 10);

}*/
