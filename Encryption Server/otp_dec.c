/****************************************
 * Filename: otp_enc.c
 * Author: Johannes Pikel
 * Date: 2017.03.02
 * ONID: pikelj
 * Class: CS344-400
 * Assigment: Program 4 OTP 
 * Description: This is the client program that will open the 
 * ciphertext and key file, send the contents to the server for encryption
 * and then write to stdout the received cipher text
 * *************************************/

#include <stdio.h>      /* standard input output */
#include <stdlib.h>     /* standard library */
#include <unistd.h>     /* constants and types */
#include <string.h>
#include <sys/types.h>      /* definition for system types */
#include <sys/socket.h>     /* so we may used sockaddr and other structs */
#include <netinet/in.h>     /* Internet address family */
#include <netdb.h>          /* network database operations */
#include <sys/stat.h>       /* so we can get the file sizes */
#include <assert.h>
#include <fcntl.h>          /* file control options */
#include <errno.h>


#define READMAX 64
#define ID 43               /* id sent to server for validation */

/* function prototypes */
void writeError(char*); /* function that writes to STDERR a message passed in */
int getFileLength(int);         /* returns a files length -1 for new line */
void compareFileLen(int, int, char*);        /* compares the first to second*/
char* readFile(int, int);                    /* reads in the contents of a file */
void verifyLine(char*, char*);               /* verifies for valid characters  */
void getErrMsg(int , int );                  /* gets error message from server */
void sendText(int , char* );                 /* sends text to server */
void getResponse(int );                      /* gets a text response from server */
int openFile(char*);                         /* opens a file returns fd */
int initServer(char*, struct sockaddr_in*);  /* initializes the sockaddr_in struct */

/****************************************
 * Function: main()
 * Parameters: 3 command line args, {textfilename} {keyfilename} {port number}
 * Description: When given the correct number of command line arguments
 * The first file should be a ciphertext filename and the second file the key file
 * that was used to encrypt the ciphertext file. The port should be the port that the
 * otp_dec_d server is listening on.
 * First compares the lengths of the two files given. If the key file is too short.
 * Prints and error and exits the program.
 * Otherwise opens the first file and reads in all but the last byte which we 
 * expect to be a newline.  
 * Then opens the second file and reads in the same number of bytes from 
 * the key file.  Opens a new connection to the server at locahost on the passed
 * port number.
 * Sends an authentication ID to make sure we are connected to the correct server
 * Then sends the ciphertext string and keytext line.
 * Finally waits for the response from the server which should be the plaintext
 * outputs this to STDOUT. 
 * Preconditions: passed 2 valid filenames and a valid port
 * Postconditions: plainttext output to STDOUT
 * *************************************/

int main(int argc, char* argv[]){

    int socketFD, inFileFD, keyFileFD, inFileLen, keyFileLen,
        response, bytesSent, bytesRead, id;

    struct sockaddr_in sevAdd;
    char* ciphertext;
    char* keytext;
    struct timeval tv;

    id = ID;
    /* we need exactly 4 arguments for this program which is inclusive of the */
    /* program name itself */
    if (argc != 4)
        writeError("USAGE: otp_dec {textfile} {keyfile} {port}\n");

    inFileFD = openFile(argv[1]); /* open both files plaintext and keytext */
    keyFileFD = openFile(argv[2]); 

    inFileLen = getFileLength(inFileFD); /* compare the lengths of the two files */
    keyFileLen = getFileLength(keyFileFD); 
    compareFileLen(inFileLen, keyFileLen, argv[2]);

    /* read in the ciphertext file and verify that it contains valid characters */
    ciphertext = readFile(inFileLen, inFileFD); 
    verifyLine(ciphertext, argv[1]);
    /* read in the key file */
    keytext = readFile(inFileLen, keyFileFD);

    /* open a connection to the decyption server at the passed in port */
    socketFD = initServer(argv[3], &sevAdd);        

    /* set a timeout on this socket so we may check against EAGAIN or EWOULDBLOCK*/
    /* on errno in the while loops that use recv() this way we don't get stuck */
    /* hopefully */
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    setsockopt(socketFD,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(struct timeval));

    /* send what we think is the server ID want to be connecting to i.e. -42 */
    /* this is the part that verifies we are connecting to the correct server */
    bytesSent = send(socketFD, &id, sizeof(int), 0);
    bytesRead = recv(socketFD, &response, sizeof(int), 0);
    /* the server response does not match out ID then we have not connected */
    /* to the correct server, wait for an error message from the server and bail */
    if(response != id){
        free(ciphertext);
        free(keytext);
        getErrMsg(socketFD, atoi(argv[3]));
    }
    else {
        /* otherwise we send the cipher text*/
        sendText(socketFD, ciphertext);
        /* wait for the server to respond that it is complete so we don't overload */
        /* the socket or miss any information */
        recv(socketFD, &response, sizeof(int), 0); 
        /* then send the key text */
        sendText(socketFD, keytext);
        /* finally we wait for the decrypted message to come back here */
        getResponse(socketFD);
    }

    /* free some heap memory and close the connection to the server */
    close(socketFD);
    free(ciphertext);
    free(keytext);
    return(0);
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
    exit(1);
}

/****************************************
 * Function: getFileLength()
 * Parameters: passed an open filedescriptor
 * Description: Seeks to the end of the file.  lseek will return the offset from the
 * beginning of the file which we can use as the number of bytes contained in the file.
 * All our files are expected to have a newline, so we want to disregard the newline
 * byte so we subtract 1.  Then reset the file pointer to the beginning of the file for
 * good measure and return the length of the file
 * Preconditions: passed open filedescriptor 
 * Postconditions: returns int
 * *************************************/

int getFileLength(int fd){

    int fileLen;

    fileLen = lseek(fd, 0, SEEK_END);
    fileLen -= 1;
    lseek(fd, 0, SEEK_SET);

    return fileLen;

}

/****************************************
 * Function: compareFileLen()
 * Parameters: int, int and char *
 * Description: The first int should be that of the ciphertext length and the sencond
 * the keytext length.  If our keytext is not long enough we print and error message.
 * We expect to receive the keytext filename as the third argument.  Passed to the 
 * program as a command line argument.
 * If the key file is too short, exit with a value of 1.
 * Otherwise do nothing.
 * Preconditions: passed two ints a char *
 * Postconditions: exit(1) if condition met
 * *************************************/

void compareFileLen(int sourceLen, int keyLen, char* msg){

    if(sourceLen > keyLen){
        fprintf(stderr, "Error: key '%s' is too short\n", msg); 
        exit(1);
    }
}

/****************************************
 * Function: readFile()
 * Parameters: int, int
 * Description: The first int is the number of bytes to read in and the second is the
 * open filedescriptor.  Allocates memory for the size +1 to account for \0 byte. Then
 * for good measure sets our file pointer to the beginning of the file and reads the
 * number bytes passed in from the filedescriptor. If for some reason we did not read
 * the correct number of bytes we write and error and exit(1).
 * We remove any newline characters in the line, which there should be none and replace
 * with a \0 byte.
 * Preconditions: provided a valid size of bytes and filedescriptor of open file
 * Postconditions: returns char * to the memory allocated to hold the string
 * *************************************/

char* readFile(int size, int fd){

    int bytesRead;
    char* line = malloc((size + 1) * sizeof(char));

    assert(line != 0);

    memset(line, '\0', (size + 1) * sizeof(char));
    
    lseek(fd, 0, SEEK_SET);   
    
    bytesRead = read(fd, line, (size * sizeof(char)));

    if(bytesRead != size){
        writeError("Error: didn't read entire file");
    }

    line[strcspn(line, "\n")] = '\0';

    return line;

}

/****************************************
 * Function: verifyLine()
 * Parameters: char*, char*
 * Description: the first c string is a pointer to the line of text we want to verify
 * the second is the filename from where this text was read.
 * Iterates through the entire line and checks to see if any of the CHAR are outside
 * the ASCII range 65 to 90 if this is the case then we also check to see if the CHAR
 * is ASCII 32 which is a space.  If none of these conditions are met then we have an
 * invalid CHAR, write an error message stderr and exit(1). Otherwise do nothing.
 * We also free the heap memory of the c string before we exit.
 * Preconditions: passed two valid char *
 * Postconditions: exit(1) if necessary or do nothing.
 * *************************************/

void verifyLine(char* text, char* file){

    int i;

    for(i = 0; i < strlen(text); i++){
        if((text[i] < 65 || text[i] > 90) && text[i] != 32){
            fprintf(stderr, "otp_dec error: %s contains bad input\n", file);
            free(text);
            exit(1);
        }
    }
}

/****************************************
 * Function: getErrMsg()
 * Parameters: int socket file descriptor, int representing port number
 * Description: This function waits to receive an integer from the server that is the
 * size of the message being sent.  Then allocates some heap memory for a char string 
 * that will temporarily store the message being sent by the server.  Once the complete
 * message has been received it outputs that message to STDERR.
 * Preconditions: receives valid open socket, and int representing the port number
 * Postconditions: prints error message to screen and exit with value 2
 * *************************************/

void getErrMsg(int socketFD, int portNum){
    int bytesRead = 0;
    int totalRead = 0;
    int recSize;

    char* msg;
    char readBuff[10];

    /* this is the size of the message we are to receive in bytes */
    recv(socketFD, &recSize, sizeof(int), 0);

    /* based on the size of the message we allocate that much space on the heap */
    msg = malloc((recSize+1) * sizeof(char));
    assert(msg != 0);

    memset(msg, '\0', (recSize+1)*sizeof(char));
    /* could probably check for != here but essentially < is the same because we */
    /* should only be getting the number of bytes as received earlier */
    while(totalRead < recSize){
        memset(readBuff, '\0', sizeof(readBuff));
        bytesRead = recv(socketFD, readBuff, sizeof(readBuff) -1, 0);
        /* out timeout will set errno after 30 seconds of no activity */
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            break;
        }
        totalRead += bytesRead;
        strcat(msg, readBuff);
    } 

    /* print the error msg, the text gotten from the server is the server name */
    /* and exit with 2 */
    fprintf(stderr, "Error: otp_dec may not connect to %s on port %d\n", msg, portNum);
    free(msg);
    close(socketFD);
    exit(2);


}

/****************************************
 * Function: sendText()
 * Parameters: int socket file descriptor, char* to a c string
 * Description: This function first sends the size of the c string to the server
 * so that the server may prepare some space on the heap.  Then it sends the 
 * c string to the server.  So far this has not given me any troubles as the server
 * has been able to keep up but perhaps it is worth in the future adding a recv()
 * between the two sends and wait for the server to be ready to accept the text message.
 * If either not enough bytes are sent or no bytes are sent then we write and error
 * message
 * Preconditions: valid open socket file descriptor, valid char* to a c string passed
 * Postconditions: c string send to the open socket.
 * *************************************/

void sendText(int socketFD, char* text){

    int bytesSent;

    int length = strlen(text);

    send(socketFD, &length, sizeof(int), 0);
    /* so far the server has been able to keep up with these two sends back to back*/
    /* but perhaps it would be good to add a recv() here to wait for some integer */    
    /* so the server could response back that it is ready to accept the c string */
    bytesSent = send(socketFD, text, (length * sizeof(char)), 0);

     if(bytesSent < 0)
         writeError("CLIENT: ERROR writing to socket");

     if(bytesSent < strlen(text))
         writeError("CLIENT: WARNING: Not all data written to socket!\n");

}

/****************************************
 * Function: getResponse()
 * Parameters: int valid socket file descriptor
 * Description: Waits for the server to send an integer that is the size of the 
 * c string we are expecting to receive.  Then we allocation some space on the heap 
 * for the c string.  We add +2 for the eventual newline character and a null byte.
 * Then keep receiving from the server while our total number of bytes read is less
 * than our expected bytes.  Could likely change this to != but this is working just
 * fine for now. Finally we write out the received c string to STDOUT appending a 
 * newline character to it first.
 * Preconditions: passed open socket file descriptor
 * Postconditions: message written to STDOUT
 * *************************************/

void getResponse(int socketFD){

    char* msg;
    char readBuff[READMAX];
    int totalRead = 0;
    int bytesRead = 0;
    int size;

    recv(socketFD, &size, sizeof(int), 0);

    msg = malloc((size+2) * sizeof(char));
    assert(msg != 0);
    memset(msg, '\0', (size+2) * sizeof(char));
    /* we read -1 size of our readBuff so we avoid the null byte */
    while(totalRead < size){
        memset(readBuff, '\0', sizeof(readBuff));
        bytesRead = recv(socketFD, readBuff, sizeof(readBuff)-1, 0);
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            break;
        }
        totalRead += bytesRead;
        strcat(msg, readBuff);
      
    }
    strcat(msg, "\n");

    write(STDOUT_FILENO, msg, strlen(msg));

    free(msg);
}

/****************************************
 * Function: openFile()
 * Parameters: char *
 * Description: given a c string of a valid filename attempts to open the file
 * check that we returned a valid file descriptor and then returns that file 
 * descriptor to the calling function.
 * Preconditions: passed valid c string of a filename
 * Postconditions: returns int of open file descriptor
 * *************************************/

int openFile(char* file){

    int fd;

    fd = open(file, O_RDONLY);
    assert(fd != -1);

    return fd;
}

/****************************************
 * Function: initServer()
 * Parameters: char *, struct sockaddr_in*
 * Description: When passed a pointer to a sockaddr_in struct initializes the
 * server to be on the localhost at the port number defined in the char* c string.
 * Returns to the caller the open server socket file descriptor.  Otherwise fails
 * if the socket cannot be opened or there is trouble connecting to the server.
 * Preconditions: valid port number, and pointer to sockaddr_in struct
 * Postconditions: returns connected in of open socket
 * *************************************/

int initServer(char* port, struct sockaddr_in* sevAdd){

    int socketFD;
    struct hostent* sevInfo;

    /* initalizing ther sockaddr_in struct here */
    memset((char*)sevAdd, '\0', sizeof(*sevAdd));

    sevAdd->sin_family = AF_INET;
    sevAdd->sin_port = htons(atoi(port));
    sevInfo = gethostbyname("localhost");

    if(sevInfo == NULL)
        writeError("CLIENT: ERROR, no such host\n");

    memcpy((char*)&sevAdd->sin_addr.s_addr, (char*)sevInfo->h_addr, sevInfo->h_length);

    socketFD = socket(AF_INET, SOCK_STREAM, 0);

    if(socketFD < 0)
        writeError("CLIENT: ERROR opening socket");
    /* attempt to connect to the server bail if it fails */
    if(connect(socketFD, (struct sockaddr*)sevAdd, sizeof(*sevAdd)) < 0)
        writeError("CLIENT: ERROR connecting");

    return socketFD;
}
