/****************************************
 * Filename: client.c
 * Author: Johannes Pikel
 * Date: 2017.07.12
 * ONID: pikelj
 * Class: CS372-400
 * Assignment: Project#1
 * Description: implementation file for the client side of the two way chat
 * Cite: Used the site for reference through out this project
 * http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 * **************************************/


#include <stdio.h>      /* standard input output */
#include <stdlib.h>     /* standard library */
#include <unistd.h>     /* constants and types */
#include <string.h>
#include <sys/types.h>      /* definition for system types */
#include <sys/socket.h>     /* so we may used sockaddr and other structs */
#include <netinet/in.h>     /* Internet address family */
#include <netdb.h>          /* network database operations */
#include <sys/stat.h>       /* so we can get the file sizes */
#include <sys/ioctl.h>      /*to get the terminal size*/
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include "pikelj_chat.h"

#define MAXLINE 500     /* max line length for input */
#define MAXHANDLE 10    /* max length for user's handle */
#define MAXCONN 5       /* max connections in server queue */
#define MAXRECV (MAXLINE + MAXHANDLE + 3 + INTEGER)
#define INTEGER 5
#define CARET "> "
#define QUIT "\\quit"
#define BACKSLASH 92

#ifdef _WIN32
#define clearScreen() system("cls")
#else 
#define clearScreen() system("clear")
#endif



/****************************************
 *  Function: main()
 *  Parameters: 1 or 2 arguments from the command line
 *  Preconditions: passed either a port number or a host and port number
 *  Postconditions: server started or client connects to server
 *  Description: Validates that either a total of 2 or 1 command line argurments
 *  were passed in.  the function name counts so we check with +1
 *  gets the users handle.  then starts the appropriate feature either
 *  the chat server or the client
 *  ***************************************/
int main(int argc, char* argv[]){
    char *handle;

    /*clearScreen();*/
    if (!(argc == 3 || argc == 2)) {
        fprintf(stderr, "Usage: $ chatroom {hostname} {portnumber}");
        fprintf(stderr, "Usage: $ chatroom {portnumber}");
        exit(1);
    }

    valArgs(argc, argv);
        
    /*set the user handle*/
    handle = getHandle();
    fprintf(stdout, "Please note messages over 500 chars will be truncated.\n");
    /* initialize either the server or the client based on the command line args */
    if(argc ==  2){
        runServer(argv[1], handle);
    } else  if (argc == 3){
        runClient(argv[1], argv[2], handle);
    }
    free(handle);
    return 0;
}

/****************************************
 *  Function: valArgs()
 *  Parameters: int, char*
 *  Preconditions: passed the number of elements in the char* and a char*
 *  Postconditions: if the port number is not valid exits(1)
 *  Description: Using atoi to convert the char string into an integer validates
 *  whether the port number is in the range from 1025 to 65535.  If it is outside
 *  that range then exits(1)
 *  ***************************************/
void valArgs(int argc, char*argv[]){
    int port;

    if (argc == 3){
        port = atoi(argv[2]);
    } else {
        port = atoi(argv[1]);
    }

    if (port < 1024 || port > 65535){
        fprintf(stderr, "Port number out of range. Valid range: 1024-65535\n");
        exit(1);
    }

}

/****************************************
 *  Function: runServer
 *  Parameters:char*, char*
 *  Preconditions: passed a valid char string that is the port number and a user handle
 *  Postconditions: server intialized and listens for clients
 *  Description: First make a copy of the port number for future use, in case the 
 *  command line argument gets destroyed.  Initalize a TCP socket, and pass that
 *  socket to the initServer socket where we'll try to bind that socket.
 *  We use gethostname() simply so we can print out our own host name for easy
 *  connecting.
 *  When a new client connects we get some information about that client such as
 *  their IP address and port number they are connecting from, print a status
 *  message to screen.  Upon succesful accept() pass the connection to the commLoop
 *  for message passing. WHen the commLoop returns close the connection but not
 *  the listen socket
 *  ***************************************/

int runServer(char* port, char*handle){
    int sock, conn;
    struct sockaddr_in address;
    struct sockaddr_in peer;
    int peerport;
    socklen_t length;
    char ipstr[INET6_ADDRSTRLEN];
    char serverName[512];
    char *portnum;
    /*make a copy for later use*/
    portnum = malloc(sizeof(char)*strlen(port));
    memset(portnum, '\0', strlen(port));
    memcpy(portnum, port, strlen(port));
    /*set out socket as a TCP socket and send it over to the initserver function
     * to initialize and bind the socket to a port*/
    sock = socket(AF_INET, SOCK_STREAM, 0);
    initServer(portnum, (struct sockaddr_in*)&address, sock);
    length = sizeof(address);
    /*just get our own servername so we can print it to screen*/
    gethostname(serverName, sizeof(serverName));

    /* this while loop will never quit except with CTL+C
     * continue listening for incoming connection and hand the accepted connection
     * off to the commLoop.  Print some information about incoming clients to
     * screen*/
    while(1){
        printf("Server listening on: %s, port: %s\n", serverName, portnum);
        fflush(stdout);
        listen(sock, MAXCONN);
        conn = accept(sock, (struct sockaddr*)&address, &length);
        getpeername(conn, (struct sockaddr*)&address, &length);
        struct sockaddr_in *peer = (struct sockaddr_in*)&address;
        peerport = ntohs(peer->sin_port);
        inet_ntop(AF_INET, &peer->sin_addr, ipstr, sizeof(ipstr));
        printf("Connected by: %s on port: %d\n", ipstr, peerport);
        if (conn < 0){
            fprintf(stderr,"Error on accept()");
        }
        commLoop(conn, handle);
        close(conn);
    }

    free(portnum);
    return 0;
}

/****************************************
 *  Function: runClient
 *  Parameters:char*, char*, char*
 *  Preconditions: Passed a valid hostname, valid portnumber, and user handle
 *  Postconditions: connection with the remote server and connection passed
 *  to the commLoop
 *  Description: upon successfuly connection to the remote server we'll set a 60
 *  second timeout, we'll get our own port number that we connecting with and
 *  send that as the initial message to the server along with out handle.
 *  then enter the commLoop where message passing happens.
 *  Finally when commLoop returns close the socket
 *  ***************************************/

int runClient(char* host, char* port, char* handle){
    int sock;
    struct sockaddr_in address;
    socklen_t length;
    struct timeval tv;
    char* portnum;

    sock = connClient(host, port, &address);
    length = sizeof(address);
    /*set timeout option so client drops any recv() after 30 seconds
     * this will set the errno to EAGAIN or EWOULDBLOCK and we can check
     * against this*/
    tv.tv_sec = 60;
    tv.tv_usec = 60;
    setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(struct timeval));
    /*so we can get our local client port number*/
    getsockname(sock, (struct sockaddr*)&address, &length);
    portnum = malloc(sizeof(address.sin_port)+1);
    sprintf(portnum, "%u\n",ntohs(address.sin_port));
    /*first send just our handle and portnumber*/
    sendText(sock, handle, portnum);
    commLoop(sock, handle);
    close(sock);
    free(portnum);
    return 0;

}

/****************************************
 *  Function: commLoop
 *  Parameters: int, char*
 *  Preconditions: passed a valid connection socket, and a char* for the user handle
 *  Postconditions: messages are passed back and forth between the client and server
 *  Description: Using the select function we query the socket to see if there is data
 *  avaiable, if so we pass the socket in to the getResponse function that handles
 *  receiving the message, then we check for the quit message before printing the message
 *  If the socket is not set we check STDIN and see if there is data there that
 *  needs to be read in.  If so we get the line, check the input line for the quit
 *  message.  Send it to the server regardless because we want to notify the server
 *  we are quitting!
 *  Extra credit:  This allows the client to send messages continuously without
 *  waiting for a response.
 *  Cite: for the use of select from this source
 *  http://beej.us/guide/bgnet/output/html/multipage/selectman.html
 *  ***************************************/
int commLoop(int sock, char*handle){
    char *message;
    int done = 0, cols, rv, flags, n;
    struct timeval tv;
    
    /*use the size of the terminal window divided by half in the print_message()
     * grab it here, so we do not change the size of our printing window once
     * we are in the commLoop, hopefully this will keep the formatting more or less
     * the same for a given connection*/
    cols = getTerminalCols()/2;
    tv.tv_sec = 0.1;
    tv.tv_usec = 10000;

    /*print our handle to start*/
    printHandle(handle);

    do {
        /*I had this outside of the while loop at first but my receive kept blocking
         * so by putting this inside the while loop I was able to get the socket
         * to not block on receive. or at least select not to think there was always
         * data on the socket*/
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);
        n = sock+1;
        /*query our possible File Descriptors, in this case we have a socket and
         * STDIN, if the socket has data get the message otherwise move on STDIN*/
        rv = select(n, &readfds, NULL, NULL, &tv);
        if(rv == -1){
            perror("select");
        } else {
            if(FD_ISSET(sock, &readfds)){
                message = getResponse(sock);
                if( checkForQuit(message) == 1){
                    done = 1;
                    fprintf(stdout, "\nConnection closed by remote\n");
                    break;
                } else {
                    printMessage(message, cols);
                    printHandle(handle);
                }
            } 
            if(FD_ISSET(STDIN_FILENO, &readfds)) {
                message = getInput(handle);
                sendText(sock, handle, message);
                if (checkForQuit(message) == 1){
                    fprintf(stdout, "\nClosing connection\n");
                    done = 1;
                    break;
                }
                printHandle(handle);
            }
        }
    } while(done == 0);
    /*once we get the quit message we want to shutdown the socket for any more
     * writting.  I did this because in server mode, if there was left over data in
     * STDIN it kept sending to the socket and when a new client connected they received
     * the stale data*/
    shutdown(sock, SHUT_WR);
    return 0;
}

/****************************************
 *  Function: printHandle
 *  Parameters: char*
 *  Preconditions: passed userhandle, can be 0 length
 *  Postconditions: handle printed to screen
 *  Description: use the fflush() so that we force the print to screen
 *  ***************************************/

void printHandle(char* handle){
    fprintf(stdout, handle);
    fflush(stdout);
}

/****************************************
 *  Function: printMessage()
 *  Parameters: char*, int
 *  Preconditions: passed a complete message and a size in cols, cols should be half the
 *  terminal window columns
 *  Postconditions: message printed to screen in a formatted method
 *  Description: first we print a newline to move past the prompt
 *  then pad out the line with spaces, print the message in sectiosn that is no more
 *  than half the columns in length, so we avoid word wrap
 *  finally if the message is longer by itself than the number columns we'll add another
 *  newling for good measure
 *  ***************************************/
void printMessage(char* msg, int cols){
    int i;
    int j;

    printf("\n");
    /*this loop iterates through the strings length in counts of the cols passed in*/
    for(i = 0; i < strlen(msg); i += cols){
        for(j = 0; j < cols-3; j++){
            fprintf(stdout, " ");
        }
        fprintf(stdout, " * ");
        /* here we print the part of the bytes of the string up to the width of the cols
         * or until the end of the string is reached*/
        for(j = 0; j < cols && j+i < strlen(msg); j++){
            fprintf(stdout, "%c", msg[i+j]);
        }
        if(strlen(msg) > cols){
            fprintf(stdout, "\n");
        }
    }
    free(msg);
}


/****************************************
 *  Function: checkForQuit
 *  Parameters: char*
 *  Preconditions: passed a char* string
 *  Postconditions: returns int
 *  Description: find the location to the first letter in the defined \quit, meaning
 *  the backslash, the find the caret meaning the >
 *  if it is a message we typed in to STDIN from this end then there is no caret
 *  because the handle is preprended later, so the backslash should be in position 0
 *  otherwise we want to make sure that the backslash begins exactly 3 characters after
 *  the location of the caret beceause we have 
 *  userhandle> \quit
 *  if all that is true, then we can compare the next 4 chars from the pointer to the 
 *  backslash to see if it matches \quit
 *  return 1 if true
 *  otherwise return 0
 *  ***************************************/
int checkForQuit(char* msg){
    int indexQuit;
    int indexCaret;
    char* ptrQuit;
    char* ptrCaret;
    ptrQuit = strstr(msg, QUIT);
    ptrCaret = strstr(msg, CARET);
    indexQuit = ptrQuit - msg;
    indexCaret = ptrCaret - msg;
    if ((indexQuit == 0 || indexQuit < indexCaret + 3)
            && indexQuit >= 0
            && strncmp((char*)ptrQuit, QUIT, 4) == 0){
        return 1;
    }
    return 0;
}

/****************************************
 *  Function: getInput  
 *  Parameters:char*
 *  Preconditions: passed valid char* string
 *  Postconditions: returns the line from STDIN
 *  Description: gets a line from STDIN.  the truncate the line to the MAXLINE of 
 *  500 bytes + 1 for the null byte.  Copy the number of characters into a new
 *  char*string and return a pointer that location in memory.
 *  ***************************************/
char* getInput(char* handle){
    char* text;
    char *newline = NULL;
    size_t size = 0;

    if(getline(&newline, &size, stdin) < 0){
        fprintf(stdout, "An error occured, try again.");
        free(newline);
        getInput(handle);
    }

    text = malloc(sizeof(char)*MAXLINE+1);
    if(size > MAXLINE){
        strncpy(text, newline, MAXLINE);
    } else{
        strcpy(text, newline);
    }
    free(newline);
    return text;
}

/****************************************
 *  Function: getTerminalCols
 *  Parameters: none
 *  Preconditions: none
 *  Postconditions: returns int
 *  Description: gets the number of columns that the terminal says it has
 *  and return that as an integer.
 *  ***************************************/
int getTerminalCols(){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

/****************************************
 *  Function: getResponse
 *  Parameters: int
 *  Preconditions: passed in a valid connected socket with data
 *  Postconditions: char* of the message received return
 *  Description: first we read in exactly 5 bytes worth of data.  Those 5 bytes
 *  will be converted into a integer for the host from network standard.
 *  That integer is the number of bytes we expect for the remaining message size.
 *  So now we can loop until we receive the remaining bytes.
 *  then return the message once complete
 *  Cite: used this idea prepending the message with the size of the message
 https://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-%20of-data
 *  ***************************************/
char* getResponse(int sock){
    char *buf;
    char *msg;
    char *pos;
    char *raw_length;
    int bytesread = 0;
    int total = 0;
    uint16_t length = 0;

    /*we expect every message to be prepended with a padded out 5 bytes that is a
     * network endian integer representing the number of bytes in the message following*/
    raw_length = malloc(sizeof(char)* INTEGER);
    buf = malloc(sizeof(char)*(MAXHANDLE));
    msg = malloc(sizeof(char)*(MAXRECV));
    memset(msg, '\0', MAXRECV);
    memset(raw_length, '\0', INTEGER);
    /*receive exactly those 5 bytes which should easily happen in 1 packet*/
    recv(sock, raw_length, INTEGER, 0);
    length = ntohs((uint16_t)atoi(raw_length));
    /*after converting it to our host integer value we know how many bytes to expect
     * before return the complete message.  loop here until all the bytes have been
     * received.  Since TCP is reliable, we can reasonably avoid the need for a 
     * complete timeout*/
    while(total < MAXRECV && total < length){
        memset(buf, '\0', MAXHANDLE);
        bytesread = recv(sock, buf, sizeof(buf), 0);
        total += bytesread;
        strcat(msg, buf);
    }

    free(raw_length);
    return msg;
}

/****************************************
 *  Function: sendText
 *  Parameters:int, char*, char*
 *  Preconditions: passed a connected socket, char* for the user handle and char* for
 *  the message to be sent
 *  Postconditions: message sent to the socket, that then transmits to the remote
 *  Description: First we get our handle + message length, that length we will
 *  convert into a network endian integer value, prepend our payload with it and then
 *  append the userhandle and the message.  Finally we continue to send the message
 *  until we have sent all the bytes including the first 5 bytes.
 *  Cite: handling partial from
 *  http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#sendall
 *  ***************************************/
void sendText(int sock, char* handle, char* message){
    int length, bytes;
    char* text;
    int total = 0;
    int bytesleft;
    int n;
    int i;
    length = strlen(handle) + strlen(message);
    bytesleft = length + INTEGER;

    text = malloc(sizeof(*text) * (INTEGER + length + 1));
    memset(text, '\0', (INTEGER + length)*sizeof(char));
    sprintf(text, "%d", htons(length));
    /*sometimes when I used htons I would get 3 bytes worth and sometimes 5 bytes worth
     * so arbitrarily pad out the string with 5 bytes.  The spaces so far have been
     * ignored on the receiving end by the ntohs()*/
    for(i = strlen(text); i < 5; i++){
        strcat(text, " ");
    }
    strcat(text, handle);
    strcat(text, message);

    /* continue sending until all the data has been pushed to the socket */
    while(total < length + INTEGER){
        n = send(sock, text+total, bytesleft, 0);
        if(n == -1){
            break;
        }
        total += n;
        bytesleft -= n;
    }

    if(n < 0){
        writeError("Client: error writing to socket");
    }
    if(total < length){
        writeError("Client: warning: all data not written to socket");
    }

    free(text);
}

/****************************************
 *  Function: getHandle
 *  Parameters:none 
 *  Preconditions:none
 *  Postconditions: returns char*
 *  Description: prompts the user for a userhandle, gets the complete line
 *  so that we clear the STDIN buffer with getline.  Then removes the newline from
 *  the end of the string for good measure and so our formatting doesn't get wierd.
 *  Finally we'll copy exactly the MAXHANDLE number of bytes to a new char* or the
 *  string if it is less.  This new char* we'll return and release the memory of the 
 *  one we used with getline.
 *  ***************************************/

char* getHandle() {
    char* handle;
    char *newline;
    char *text = NULL;
    size_t size = 0;

    fprintf(stdout, "Please enter a handle (Limit 10 chars): ");
    if(getline(&text, &size, stdin) < 0){
        fprintf(stdout, "An error occured, try again.");
        free(text);
        getHandle();
    }
    /*remove the newlines*/
    if ((newline=strchr(text, '\n')) != NULL){
        *newline = '\0';
    }
    handle = malloc(sizeof(char)*MAXHANDLE+3);/*to accomodate the CARET*/
    memset(handle, '\0', MAXHANDLE+3);

    if(size > MAXHANDLE){
        strncpy(handle, text, MAXHANDLE);
    } else {
        strcpy(handle, text);
    }
    strcat(handle, CARET);
    free(text);

    return handle;
}


/****************************************
 *  Function: clearInBuffer 
 *  Parameters:none
 *  Preconditions:none
 *  Postconditions: clears STDIN buffer
 *  Description: Left this here, I don't think I use this.  It works too well.
 *  when it runs I have to hit enter twice to execute a command.
 *  ***************************************/
void clearInBuffer(){
    char c;
    while ((c = getchar()) != EOF && (c != '\n')) {}
}

/****************************************
 * Function: connClient()
 * Parameters: char *, struct sockaddr_in*
 * Description: When passed a pointer to a sockaddr_in struct initializes the
 * server to be on the localhost at the port number defined in the char* c string.
 * Returns to the caller the open server socket file descriptor.  Otherwise fails
 * if the socket cannot be opened or there is trouble connecting to the server.
 * Preconditions: valid port number, and pointer to sockaddr_in struct
 * Postconditions: returns connected in of open socket
 * Cite: This function originally written by me, Johannes Pikel for
 * the final project on CS344-400
 * *************************************/

int connClient(char* hostname, char* port, struct sockaddr_in* sevAdd){

    int sock;
    struct hostent* sevInfo;

    /* initalizing ther sockaddr_in struct here */
    memset((char*)sevAdd, '\0', sizeof(*sevAdd));

    sevAdd->sin_family = AF_INET;
    sevAdd->sin_port = htons(atoi(port));
    sevInfo = gethostbyname(hostname);

    if(sevInfo == NULL)
        writeError("CLIENT: ERROR, no such host\n");

    /* now we'll copy the hostent* to localhost into our sockaddr_in struct */
    memcpy((char*)&sevAdd->sin_addr.s_addr, (char*)sevInfo->h_addr, sevInfo->h_length);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0)
        writeError("CLIENT: ERROR opening socket");

    /* attempt to connect to the server, bail if it fails */
    if(connect(sock, (struct sockaddr*)sevAdd, sizeof(*sevAdd)) < 0)
        writeError("CLIENT: ERROR connecting");

    return sock;
}

/****************************************
 * Function: initServer()
 * Parameters: char*, struct sockaddr_in*, int socket file descriptor
 * Description: Initializes the server to listen on the passed in portnumber as the
 * char* that should have been received as a command line argument. Then attempts to
 * bind the server on this port number for listening for clients and binds that port
 * to the file descriptor.
 * Preconditions: passed in valid portnumber, sockaddr_in struct *, and socket FD
 * Postconditions: port is bound to the socket file descriptor
 * so that our main function can listen for incoming connections on that socket
 * *************************************/

void initServer(char* port, struct sockaddr_in* serverAdd, int sock){
    int yes = 1;

    memset((char*)serverAdd, '\0', sizeof(*serverAdd));

    serverAdd->sin_family = AF_INET;
    serverAdd->sin_port = htons(atoi(port));
    serverAdd->sin_addr.s_addr = INADDR_ANY;

    if(sock < 0)
        writeError("ERROR opening socket");

    if(bind(sock, (struct sockaddr*)serverAdd, sizeof(*serverAdd)) < 0)
        writeError("ERROR on bind()");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

}


/****************************************
 * Function: writeError()
 * Parameters: char* 
 * Description: Writes the c string to STDERR. And bails out of the program with 
 * exit(1)
 * Preconditions: receives a char * to a string literal or c string
 * Postconditions: writes to stderr and exits with value 1
 * *************************************/

void writeError(char* msg){
    write(STDERR_FILENO, msg, strlen(msg));
    puts("---Goodbye---");
    exit(1);
}
