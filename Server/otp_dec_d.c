/****************************************
 * Filename: otp_enc_d.c
 * Author: Johannes Pikel
 * Date: 2017.03.01
 * ONID: pikelj
 * Class: CS340-400
 * Assignment: Program 4 OTP
 * Description: THis will create a server that will decrypt text sent to it via a
 * one-time-pad encryption method and return to the client the decrypted text
 * Reference: used the 4.3 Network Servers Lecture in CS344 multiserver.c
 * Reference: http://man7.org/tlpi/code/online/dist/sockets/is_echo_v2_sv.c.html
 * Reference: http://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
 * **************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>         /* for memset() */
#include <unistd.h>
#include <sys/types.h>      /* definition for system types */
#include <sys/socket.h>     /* so we may used sockaddr and other structs */
#include <netinet/in.h>     /* Internet address family */
#include <assert.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define READMAX 64
#define MAXCONN 10              /* max connections on listen socket */
#define SERVERNAME "otp_dec_d"  /* server name */
#define SID 43                  /* used for client validation */

/* function prototype */
void writeError(char* msg); /* function that writes to STDERR a message passed in */
int findIdx(char);
void initServer(char*, struct sockaddr_in*, int);/* init the sockaddr_in struct */
int validateClient(int);                /* validates the client */
char* getText(int);                          /* receives text */
char* convert(char*, char*);            /* converts to ciphertext */
static void childHandler(int);
void handleRequest(int);


/****************************************
 * Function: main()
 * Parameter: portnumber passed in command line
 * Description: checks to make sure we received the appropriate number of arguments
 * In this case 2, the filename of the program and a port number.
 * Then intializes a sigaction for SIGCHLD so that the child processes can be reaped
 * when they complete. Avoid fork bombs.
 * Then initializes and listen for incoming connections on the socket bound to the 
 * port number.
 * If we get an incoming connection, fork a child process, handle the request in the
 * child and the main program continues to listen on the listen port.
 * Preconditions: none
 * Postconditions: decryption server set to listen on the specified port number
 * *************************************/

int main(int argc, char* argv[]){

    int listSockFD, connFD;

    socklen_t sizeClientInfo;
    struct sockaddr_in serverAdd, clientAdd;
    struct sigaction sa;

    if(argc < 2){
        fprintf(stderr, "USAGE: %s {port} [&]\n", argv[0]);
        exit(1);
    }

    /* we don't need to track child PIDS because we will reap the anonymously when*/
    /* they finish so no need to track PIDS at this time */
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = childHandler;
    sigfillset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    listSockFD = socket(AF_INET, SOCK_STREAM, 0);

    /* initalize our server and listen on the port number*/
    initServer(argv[1], (struct sockaddr_in*) &serverAdd, listSockFD);

    listen(listSockFD, 5);

    while(1){
        sizeClientInfo = sizeof(clientAdd);

        connFD = accept(listSockFD, (struct sockaddr*)&clientAdd, &sizeClientInfo);
        if(connFD < 0)
            writeError("ERROR on accept()");
        /* once we've accepted a new connection fork a child process to handle the */
        /* request.  in the child process we can go ahead and close the listen socket*/
        /* because it is not necessary*/
        switch(fork()){
            case -1:
                close(connFD);
                fprintf(stderr, "fork() failed, connection dropped");
                break;
            case 0:
                close(listSockFD);
                handleRequest(connFD);
                close(connFD);
                _exit(EXIT_SUCCESS);
                break;
            default:
                close(connFD);
                break;
        }
    }
    
    close(listSockFD);
    return(0);
}

/****************************************
 * Function: writeError() 
 * Parameters: char*
 * Description: writes the passed in c string to stderr and then exits with a value
 * of 1.
 * Preconditions: passed c string pointer
 * Postconditions: writes to stderr and exits
 * *************************************/

void writeError(char* msg){
    write(STDERR_FILENO, msg, strlen(msg));
    exit(1);
}

/****************************************
 * Function: findIdx()
 * Parameters: char
 * Description: iterates through the list of capital letters with a space and returns
 * the index location that matches the passed in char
 * Preconditions: passed char byte
 * Postconditions: returns index location or else returns -1
 * *************************************/

int findIdx(char c){
    int i = 0;
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    for(i = 0; i < 27; i++){
        if( c == letters[i] )
            return i;
    }    

    return(-1);
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

void initServer(char* port, struct sockaddr_in* serverAdd, int socketFD){

    memset((char*)serverAdd, '\0', sizeof(*serverAdd));
    
    serverAdd->sin_family = AF_INET;
    serverAdd->sin_port = htons(atoi(port));
    serverAdd->sin_addr.s_addr = INADDR_ANY;

    if(socketFD < 0)
        writeError("ERROR opening socket");

    if(bind(socketFD, (struct sockaddr*)serverAdd, sizeof(*serverAdd)) < 0)
        writeError("ERROR on bind()");

}

/****************************************
 * Function: validateclient()
 * Parameters: int open socket to client
 * Description: First receives an integer that the client passes to the server as an
 * ID that we check against the server's ID.  If these do not match we send back our
 * Server's ID to the client and then continue to send the server name back to the
 * client so that our client may print an error message that it could not connect to
 * this particular server.  Obviously for security it would be unwise to send the 
 * SERVER ID number back to the client for checking and instead send a boolean or 0 or 1.
 * But it works fine in this instance.
 * Preconditions: passed open socket to client program
 * Postconditions: if the client ID checks out do nothing, otherwise send and error 
 * message in the form of the Server's name.
 * *************************************/

int validateClient(int connFD){

    int id, sendSize, bytesSent, bytesRead;

    int sid = SID;

    bytesRead = recv(connFD, &id, sizeof(int), 0);

    if(id != sid){
        bytesSent = send(connFD, &sid, sizeof(int), 0);
        sendSize = strlen(SERVERNAME);
        bytesSent = send(connFD, &sendSize, sizeof(int), 0);
        bytesSent = send(connFD, SERVERNAME, sendSize, 0);
        return 0;
    }
    
    return 1;

}

/****************************************
 * Function: getText()
 * Parameters: int of open socket to client
 * Description: First waits to receive a size of bytes being sent by the client as an
 * integer. Then allocates space on the heap for a char string + 1 for the null byte.
 * Then recv the entire string incrementally until we've received all the bytes we 
 * expect to receive.  I realize this could fail horribly if an incorrect number of
 * bytes are being sent or a connection fails.  A timeout function would be good such
 * that if this child process does not finish in x time it is automatically killed so
 * we don't get stuck in this loop forever.  But in this instance it seems to work ok.
 * EDIT** I added a setsockopt to the handlrequest that should set a timeout of 30
 * seconds in which case this recv() should bail if the errno is set either of the
 * two flags below 
 * Preconditions: passed int of socket
 * Postconditions: returns pointer to the char string that was received
 * *************************************/

char* getText(int connFD){

    int recSize, totalRead, bytesRead;
    char readBuff[READMAX];
    char* line;

    recv(connFD, &recSize, sizeof(int), 0);

    line = malloc((recSize+1) * sizeof(char));
    assert(line != 0);

    memset(line, '\0', (recSize +1)*sizeof(char));            

    totalRead = 0;
    while(totalRead != recSize){
        memset(readBuff, '\0', sizeof(readBuff));
        bytesRead = recv(connFD, readBuff, sizeof(readBuff)-1, 0);
        /* if our timeout activated it should set ERRNO to one of these flags */
        /* so we want to bail! this was set with the setsockopt in handlerequest()*/
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            break;
        }
        totalRead += bytesRead;
        strcat(line, readBuff);
    }

    return line;

}

/****************************************
 * Function: convert()
 * Parameters: char*, char*
 * Description: 
 * Using the passed in ciphertext and the keytext, converts each letter or space
 * back into its original letter.  Assuming the same keytext has been received that
 * was used to make the cipher.  Allocates some space on the heap for the plaintext
 * and returns a pointer to the converted string.
 * Uses the findIdx() function to find the index of the letter and then follows this
 * formula
 * cipher[letter] - key[letter].  If the result is less than 0 we add the result to 27
 * The array of letters is all caps from A to Z with a space where A = index 0.
 * Preconditions: passed ciphertext with keytext used to make the cipher
 * Postconditions: returns pointer to the converted plaintext string
 * *************************************/

char* convert(char* ciphertext, char* keytext){

    int i, idx;
    char* plaintext;
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    int recSize = strlen(ciphertext);

    plaintext = malloc((recSize + 1) * sizeof(char));
    assert(plaintext != 0);
    memset(plaintext, '\0', (recSize+1)*sizeof(char));

    for(i = 0; i < recSize; i++){
        idx = (findIdx(ciphertext[i]) - findIdx(keytext[i]));
        if(idx < 0)
            idx += 27;
    
         plaintext[i] = letters[idx];
    }

    return plaintext;
}

/****************************************
 * Function: childHandler()
 * Parameters: int
 * Description: THis function is called in the sigaction for the SIGCHLD.
 * While there are any children to be reaped we reap them, indiscrimanantly. 
 * We do not worry about PIDS because all we want to do is get rid of all the children
 * But do nothing otherwise. 
 * Preconditions: SIGCHLD was caught
 * Postconditions: child process reaped
 * *************************************/

static void childHandler(int signo){
    while(waitpid(-1, NULL, WNOHANG) > 0){}
}

/****************************************
 * Function: handleRequest()
 * Parameters: int open socket file descriptor
 * Description: When this listen server receives a new incoming request a child 
 * process is forked and calls this funciton that will handle our request.
 * The server checks that the correct client is calling in with the validateClient()
 * If so we send back our server ID to acknowledge to the program that we are
 * accepting the connection.  Then get the ciphertext, send acknowledgement of
 * receipt completition in the form of an int.  Then get the keytext, convert the
 * ciphertext into plaintext.  Send the client the size of the response as an int
 * and finally send the text to the client.  Of course make sure to free the heap 
 * memory allocated
 * EDIT: added a timeval with setsockopt() to a timeoutval for this socket. So this
 * way if we ever get stuck in a recv() function we bail out and don't get stuck in
 * an endless loop.  30 seconds is a long time, but it worked when I tested at 1 second
 * and set one of my recv() loops to while(1){}
 * Preconditions: passed a valid open socket connected to the client
 * Postconditions: returns the decrypted string
 * Reference:http://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
 * *************************************/

void handleRequest(int connFD){

    int bytesSent;
    char* ciphertext;
    char* keytext;
    char* plaintext;
    int sid = SID;
    int length;
    struct timeval tv;  /*use this struct for our timeout of the socket */

    tv.tv_sec = 30;  /* 30 seconds till timeout of this socket */
    tv.tv_usec = 0;  /* 30 seconds should be a generous amount of time in this case*/
    /* this should set our socket to timeout after 30 seconds */
    setsockopt(connFD, SOL_SOCKET, SO_RCVTIMEO,(const char*)&tv, sizeof(struct timeval));
    if(validateClient(connFD)){
        /* send server id to validate client */ 
        bytesSent = send(connFD, &sid, sizeof(int), 0);

        ciphertext = getText(connFD);
        /* client is waiting on server before sending keytext send sid to */
        /* request that the client send the keytext */
        send(connFD, &sid, sizeof(int), 0);
        /* get the keytext */
        keytext = getText(connFD);
        
        plaintext = convert(ciphertext, keytext);
        length = strlen(plaintext);
        /* after converting our cipher into human readable. send the clien the size*/
        /* of the response as an int and then send along the text */
        send(connFD, &length, sizeof(int), 0);
        bytesSent = send(connFD, plaintext, strlen(plaintext), 0);
    
        free(ciphertext);
        free(plaintext);
        free(keytext);
    }
}
