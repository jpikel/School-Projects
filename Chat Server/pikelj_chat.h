/****************************************
 * Filename: client.h
 * Author: Johannes Pikel
 * Date: 2017.07.12
 * ONID: pikelj
 * Class: CS372-400
 * Assignment: Project#1
 * Description: delcaration file for the client side of the two way chat
 * **************************************/


#ifndef CLIENT_H
#define CLIENT_H

/*validates that the port number is indeed in the correct range*/
void valArgs(int argc, char*argv[]);
/* setups the server, listens for connections and passes accepted connections to
 * the commLoop*/
int runServer(char* port, char*handle);
/* connects the client to a host at a port number passed in, if the connection is 
 * accepted, then passes the socket to the commLoop*/
int runClient(char* host, char* port, char* handle);
/* handles STDIN and receiving messages between two connected hosts*/
int commloop(int socketFD, char*handle);
/* prints the passed in userhandle, flushes STDOUT*/
void printHandle(char*handle);
/* prints a formatted message, cols should be half the terminal width, so the message
 * is printed on the right half of the terminal*/
void printMessage(char* msg, int cols);
/* checks for the '\quit' message that should be the first thing in the line
 * if for instance 'a\quit' is passed in, it will not evaluate to true*/
int checkForQuit(char* message);
/* gets user input from STDIN and returns a pointer to the string */
char* getInput(char* handle);
/* returns the number of columns in the current terminal window */
int getTerminalCols();
/* reads 5 bytes from a socket, the first 5 bytes are the length of the remaining
 * message, then continues to receive on the socket until all the bytes are read
 * the first 5 bytes are formatted as network endian*/
char* getResponse(int socketFD);
/* prepends the payload with the length of the handle and message in network endian
 * then continues to send to the socket until all the bytes have been sent*/
void sendText(int socketFD, char* handle, char* message);
/* prompts the user to enter a usr handle and returns a pointer to the string */
char* getHandle();
/* loops until the EOF or '\n' is found on STDIN*/
void clearInBuffer();
/* initalizes a socket at a given port number, expects a TCP socket file descriptor
 * binds the file desciptor to the port*/
void initServer(char*port, struct sockaddr_in* address, int socket);
/* attempts to connect to the host at the passed hostname and port number, if
 * successful returns a integer that is the socket file descriptor*/
int connClient(char* hostname, char* port, struct sockaddr_in* serverAddress);
/* writes the message to screen and exits(1) */
void writeError(char* msg);

#endif
