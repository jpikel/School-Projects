/****************************************
 * Filename: pikelj_ftserver.c
 * Author: Johannes Pikel
 * Date: 2017.08.02
 * ONID: pikelj
 * Class: CS372-400
 * Assignment: Project#2
 * Description: This is the implementation file that starts and runs
 * file transfer server.  Currently it has 2 features it can return
 * the list of files in the current directory and it can transmit a file
 * requested.
 * -l is the list files
 * -g requests a file
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
#include <arpa/inet.h>      /* for the inet_ntop function*/
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <dirent.h>         /* to provide file structure access */
#include <sys/stat.h>       /* to provide stats on the file */
#include <math.h>
#include <signal.h>
#include "pikelj_ftserver.h"

#define CHUNKSIZE 10    /* max length for user's handle */
#define MAXCONN 5       /* max connections in server queue */
#define MAXRECV 1024
#define INTEGER 5

#ifdef _WIN32
#define clearScreen() system("cls")
#else 
#define clearScreen() system("clear")
#endif

/* used in our loop for the listen socket and signal handler */
volatile sig_atomic_t keepRunning = 1;

/****************************************
 *  Function: main()
 *  Parameters: 1 argument from the command line
 *  Preconditions: passed a port number
 *  Postconditions: server started 
 *  Description: Validates that 1 command line argurments
 *  were passed in.  the function name counts so we check with +1
 *  gets the users handle.  then starts ftp server 
 *  ***************************************/
int main(int argc, char* argv[]){

    struct sigaction a;
    struct sigaction brokenPipe;

    if (!(argc == 2)) {
        fprintf(stderr, "Usage: $ chatroom {portnumber}");
        exit(1);
    }

    a.sa_handler = SIGINTHandler;
    a.sa_flags = 0;
    sigemptyset( &a.sa_mask);
    sigaction(SIGINT, &a, NULL);

    brokenPipe.sa_handler = SIGPIPEHandler;
    brokenPipe.sa_flags = 0;
    sigemptyset( &brokenPipe.sa_mask);
    sigaction(SIGPIPE, &brokenPipe, NULL);


    if(valPort(argv[1])){
        exit(1);
    }
    startup(argv[1]);
    return 0;
}

/****************************************
 *  Function: valPort()
 *  Parameters: int, char*
 *  Preconditions: passed the number of elements in the char* and a char*
 *  Postconditions: if the port number is not valid returns 1
 *  Description: Using atoi to convert the char string into an integer validates
 *  whether the port number is in the range from 1025 to 65535.  If it is outside
 *  that range then exits(1)
 *  ***************************************/
int valPort(char*portchar){
    int port;

    port = atoi(portchar);

    if (port < 1024 || port > 65535){
        fprintf(stderr, "Port number out of range. Valid range: 1024-65535\n");
        return 1;
    }
    return 0;

}

/****************************************
 *  Function: startup
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
 *  message to screen.  Upon succesful accept() pass the connection to the handleRequest
 *  for service completition.
 *  ***************************************/

int startup(char* port){
    int sock, conn;
    struct sockaddr_in address;
    char serverName[512];
    char *portnum;
    char *host;
    /*make a copy for later use*/
    portnum = malloc(sizeof(char)*strlen(port));
    memset(portnum, '\0', strlen(port));
    memcpy(portnum, port, strlen(port));
    /*set out socket as a TCP socket and send it over to the initserver function
     * to initialize and bind the socket to a port*/
    sock = socket(AF_INET, SOCK_STREAM, 0);
    initServer(portnum, (struct sockaddr_in*)&address, sock);
    /*just get our own servername so we can print it to screen*/
    gethostname(serverName, sizeof(serverName));

    /* this while loop will never quit except with CTL+C
     * continue listening for incoming connection and hand the accepted connection
     * off to the commLoop.  Print some information about incoming clients to
     * screen*/
    while(keepRunning){
        printf("\nServer listening on: %s, port: %s\n", serverName, portnum);
        fflush(stdout);
        listen(sock, MAXCONN);
        conn = waitForAccept(sock, &address, &host);
        if (conn > 0){
            handleRequest(conn, host);
        }
        /*       close(conn);*/
        free(host);
    }
    printf("\nClosing listen socket\n");
    close(sock);
    free(portnum);
    return 0;
}

/****************************************
 *  Function: SIGINTHandler
 *  Parameters: int
 *  Preconditions: called from SIGINT
 *  Postconditions: keepRunning global volatile set to 0
 *  Description: a basic signal handler that sets the keepRunning to 0
 *  Reference: https://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
 *  ***************************************/
void SIGINTHandler(int sig){
    keepRunning = 0;
}

void SIGPIPEHandler(int sig){
    write(STDOUT_FILENO, "Something went wrong with the socket!", 38);
}

/****************************************
 *  Function: waitForAccept
 *  Parameters:int, struct sockaddr_in *, char**
 *  Preconditions: passed a valid bound socket, sockaddr_in* and char**
 *  Postconditions: returns int for the accepted socket
 *  Description: waits for an incoming connection to the listen port,
 *  then prints a message to stdout about the incoming connection
 *  stores the hosts name in the passed in char** host and the sockaddr_in information
 *  ***************************************/

int waitForAccept(int sock, struct sockaddr_in* address, char** host){
    int conn = -1;
    int peerport;
    struct sockaddr_in *peer;
    socklen_t length;
    char ipstr[INET6_ADDRSTRLEN];
    length = sizeof(address);
    conn = accept(sock, (struct sockaddr*)&address, &length);
    getpeername(conn, (struct sockaddr*)&address, &length);
    peer = (struct sockaddr_in*)&address;
    peerport = ntohs(peer->sin_port);
    inet_ntop(AF_INET, &peer->sin_addr, ipstr, sizeof(ipstr));
    /* we only want to print messages if keepRunning is set 1 otherwise
     * we are dealing with a SIGINT from the signal handler so we just want
     * to exit quietly*/
    if (conn < 0 && keepRunning == 1){
        fprintf(stderr,"Error on accept() Error:%d", errno);
    } else if (keepRunning == 1){
        printf("Connected from %s on port: %d\n", ipstr, peerport);
    }
    /* storing the host ip address here for later use by other functions */
    *host = malloc(INET6_ADDRSTRLEN * sizeof(char));
    memset(*host, '\0', INET6_ADDRSTRLEN * sizeof(char));
    sprintf(*host, "%s", ipstr);

    return conn;
}

/****************************************
 *  Function: handleRequest
 *  Parameters: int, char*
 *  Preconditions: passed a valid socket that is the command socket, and
 *  a char* that is the remote host
 *  Postconditions: request handled for the incoming message
 *  if we receive the command -l we'll gather all the file names in the current
 *  directory and send them back to the client 
 *  if we receive the command -g we'll go and check if the file requested exists
 *  send the filesize and then send the contents of the file to the client
 *  otherwise we'll send a message stating invalid command back to the client
 *  Description: Using strtok we 
 *  ***************************************/

void handleRequest(int sock, char* host){
    char* message;
    char* response;
    char* command;
    char* port;
    char* filename;
    char* fileSize;

    message = getResponse(sock);
    if((command = strtok(message, " ")) != NULL){
        /* if we want the list of files in the directory */
        if (strncmp(command, "-l", 2) == 0){
            port = strtok(NULL, " " );
            if(!(valPort(port))){
                printf("List directory requested on port %s.\n", port);
                response = collectFiles();
                printf("Sending directory contents to %s:%s\n", host, port);
                sendDirectory(host, port, response);
                free(response);
            } else {
                fprintf(stderr, "Client port invalid.");
            }
        /* if we want to get a file */
        } else if (strncmp(command, "-g", 2) == 0 ){
            filename = strtok(NULL, " ");
            port = strtok(NULL, " ");
            if(!(valPort(port))){
                fileSize = getFileSize(filename);
                printf("File \"%s\" requested on port %s.\n", filename, port);
                printf("File size:%s\n", fileSize);
                sendText(sock, (char*)fileSize, strlen(fileSize));
                if (strcmp(fileSize, "-1") == 0){
                    printf("File not found. Sending error message to %s:%s\n", host, port);
                } else {
                    sendContents(host, port, filename, sock);
                }
                free(fileSize);
            } else {
                fprintf(stderr, "Client port invalid.");
            }
            /* everything else at the moment is invalid */
        } else {
            printf("Invalid command. Sending error message to %s\n", host);
            sendText(sock, "INVALID COMMAND.", 16);
        }
    }
    free(message);
}

/****************************************
 *  Function: getFileSize()
 *  Parameters: char*
 *  Preconditions: string passed in that is a filename
 *  Postconditions: returns a char* that is the file size
 *  Description: iterates through the files in the current directory and finds the
 *  file that matches our argument.  Then sets file size to the size of the file after 
 *  we get fill out the struct stat.
 *  To convert it to a char* string, we alloc enough memory to hold all the digits
 *  plus 1, and then cast the integer to a string.  This way we can easily return
 *  this and it be sent to the client
 *  Reference: https://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
 *  https://stackoverflow.com/questions/3068397/finding-the-length-of-an-integer-in-c
 *  ***************************************/
char* getFileSize(char* filename){
    DIR *dir;
    struct dirent *ent;
    int fileSize = -1;
    struct stat st;
    int digits;
    char* size;

    if((dir = opendir(".")) != NULL){
        while ((ent = readdir(dir)) != NULL){
            if (strcmp(ent->d_name, filename) == 0){
                if(stat(ent->d_name, &st) == 0){
                    fileSize = st.st_size;
                    break;
                }
            }
        }
        closedir(dir);
    }
    if (fileSize > 0){
        digits = floor(log10(abs(fileSize))) + 1;
    } else {
        digits = 2;
    }

    size = malloc(digits*sizeof(char));
    memset(size, '\0', digits*sizeof(char));
    sprintf(size, "%d", fileSize);

    return size;

}

/****************************************
 *  Function: collectFiles
 *  Parameters:none
 *  Preconditions:none
 *  Postconditions: returns char*
 *  Description: Iterates through the current files in the current folder and
 *  compiles a string of the all the filenames, separated by a newline character
 *  then returns a pointer to this string.  Freeing of this string string done
 *  by others.
 *  Reference used: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
 *  https://stackoverflow.com/questions/8164000/how-to-dynamically-allocate-memory-space-for-a-string-and-get-that-string-from-u
 *  ***************************************/

char* collectFiles(){
    DIR *dir;
    struct dirent *ent;
    char* contents;
    int size;

    contents = malloc(10 * sizeof(char));
    memset(contents, '\0', 10*sizeof(char));
    size = 10;

    if ((dir = opendir(".")) != NULL){
        while ((ent = readdir (dir)) != NULL){
            if (strncmp(ent->d_name, ".", 1) != 0){
                /* if our memory allocation is too small realloc
                 * realloc is supposed to keep the original data contained in the
                 * memory the same so no need to copy it elsewhere*/
                if ( size + strlen(ent->d_name) > size - 1){
                    contents = realloc(contents, strlen(contents) + strlen(ent->d_name) 
                            + size);
                }
                strcat(contents, ent->d_name);
                strcat(contents, "\n");

            }
        }
        closedir(dir);
    } else {
        perror("Could not open directory");
    }

    return contents;

}


/****************************************
 *  Function: sendContents
 *  Parameters: char*, char*, char*, int
 *  Preconditions: passed a valid host and port, filename and cmdSock
 *  Postconditions: contents of the file sent
 *  Description: Opens the file and waits for the client send a READY message to 
 *  signify that it is ready to receive the contents of the file.
 *  Then reads 1025 bytes of the file and sends that off to sendText to be sent to
 *  the client.  Until we reach the end of the file.
 *  References: http://www.cplusplus.com/reference/cstdio/fopen/
 *  https://stackoverflow.com/questions/1835986/how-to-use-eof-to-run-through-a-text-file-in-c
 *  ***************************************/

void sendContents(char* host, char* port, char* filename, int cmdSock){
    int sock;
    struct sockaddr_in address;
    /*socklen_t length;*/
    char readBuffer[1024];
    FILE* fp;
    int size;
    char* reply;

    memset(readBuffer, '\0', sizeof(readBuffer));
    fp = fopen(filename, "rb");
    /* the client may be dealing with a duplicate file name so we'll wait
     * before we bombard the client with a bunch of data!*/
    reply = getResponse(cmdSock);
    if(strcmp(reply, "READY") == 0 ){
        printf("Sending \"%s\" to %s:%s\n", filename, host, port);
        if(fp != NULL){
            /* not until we've successfully opened the file do we attempt
             * to make a connection to the client's data port*/
            sock = connClient(host, port, &address);
            if (sock > 0){
                /*length = sizeof(address);*/
                /* read in 1024 bytes from the file, send it to the client's data port
                 * then make sure to set the entire readBuffer back to null byte*/
                while((size = fread(readBuffer, sizeof(char), 1023, fp)) > 0 ){
                    sendText(sock, readBuffer, size);
                    memset(readBuffer, '\0', sizeof(readBuffer));
                }
            }
            fclose(fp);
            close(sock);
        }
    }
}


/****************************************
 *  Function: runClient
 *  Parameters:char*, char*, char*, char*
 *  Preconditions: Passed a valid hostname, valid portnumber, and message
 *  Postconditions: file directory sent to host at port specified
 *  Description: makes a new connection to the host at the port passed in to the
 *  function.  Then proceeds to send the entirity of the text sent in the message
 *  passed in
 *  ***************************************/

int sendDirectory(char* host, char* port, char* message){
    int sock;
    struct sockaddr_in address;
    /* connect to the remote data connection here */
    sock = connClient(host, port, &address);
    if(sock > 0){
        sendText(sock, message, strlen(message));
        close(sock);
    }
    return 0;

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
    char *raw_length;
    int bytesread = 0;
    int total = 0;
    uint16_t length = 0;

    /*we expect every message to be prepended with a padded out 5 bytes that is a
     * network endian integer representing the number of bytes in the message following*/
    raw_length = malloc(sizeof(char)* INTEGER);
    buf = malloc(sizeof(char)*(CHUNKSIZE));
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
        memset(buf, '\0', CHUNKSIZE);
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
void sendText(int sock, char* message, int size){
    int length;
    char* text;
    int total = 0;
    int bytesleft;
    int n;
    int i, j;
    if(sock > 0){
        length = size;
        if (length == 0){
            length = sizeof(message);
        }
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
        for(i = strlen(text), j = 0; j < size; i++, j++){
            text[i] = message[j];
        }

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
            fprintf(stderr, "Client: error writing to socket");
        }
        if(total < length){
            fprintf(stderr, "Client: warning: all data not written to socket");
        }

        free(text);
    }
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
        fprintf(stderr, "ERROR, no such client host\n");

    /* now we'll copy the hostent* to localhost into our sockaddr_in struct */
    memcpy((char*)&sevAdd->sin_addr.s_addr, (char*)sevInfo->h_addr, sevInfo->h_length);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0)
        fprintf(stderr, "ERROR opening socket to client\n");

    /* attempt to connect to the server, bail if it fails */
    if(connect(sock, (struct sockaddr*)sevAdd, sizeof(*sevAdd)) < 0)
        fprintf(stderr, "ERROR connecting to client %d\n", sock);

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
