"""
Filename: server.py
Author: Johannes Pikel
Date: 2017.07.12
Class: CS372-400
Assignment: Project #1
Description: a server script that waits for a connection and allows the user
to respond up to 500 characters at a time to the client
Cite: General reference used from this site:
    https://docs.python.org/2/library/socket.html
"""

from __future__ import print_function
import socket   #gives us access to the connections
import sys      #for access to the commandline arguments
import os       #so we have access to the terminal size
try:
    import __builtin__ # py 2 and 3 input compatible
    input = getattr(__builtin__, 'raw_input')
except (ImportError, AttributeError):
    pass
import re       #strip away the letters
import platform
import select
import fcntl

MAX_MESSAGE = 500
MAX_HANDLE = 10
MAX_RECV = (MAX_MESSAGE + MAX_HANDLE + 3)
CHUNK_SIZE = 50
CARET = '> '
QUIT = '\quit'
NEWLINE = '\n'

def main(argv):
    """
    Function ~ main()
    Parameters ~ command line either 1 or 2 
    Preconditions ~ Provide either just a portnumber or a host and portnumber
    Postconditions ~ chatroom is set as server or connects as client
    Description ~ Checks if the addtional command line arguments are meant to set
    a server in which case only a port number is passed, or as a client in which case
    a host and port number are passed in.  Also gathers the user's handle to be used
    later
    """
    #if platform.system() == 'Linux':      
     #   os.system('clear')
#    elif platform.system() == 'Windows':
 #       os.system('cls')
    port = valArguments(argv)
    user_handle = getHandle()
    print ('Please note messages over 500 chars will be truncated.')
    if len(argv) == 2:
        sock = initServer(port)
        serverLoop(sock, port, user_handle)
    elif len(argv) == 3:
        sock = initClient(argv[1], port)
        clientLoop(sock, user_handle)

    return 0


def valArguments(argv):
    """
    Function ~ valArguments
    Parameters ~ array of char* from command line
    Preconditions ~ passed command line arguments
    Postconditions ~ port number returned
    Description ~ validates that either 2 or 3 command line arguments passed in
    validates that the port number passed whether it is in position 1 or 2 is a
    valid number, checks that it is in the valid useable range and returns it
    """
    if not (len(argv) == 3 or len(argv) == 2):
        print ('Usage: $ python pikelj_chat.py {portnumber}')
        print ('Usage: $ python pikelj_chat.py {hostname} {portnumber}' )
        sys.exit()

    if len(argv) == 2 and argv[1].isdigit():
        port = number(argv[1])
    elif len(argv) == 3 and argv[2].isdigit():
        port = number(argv[2])
    else:
        print ('Portnumber is not a valid digit')
        sys.exit()

    if port < 1024 or port > 65535:
        print ('Invalid Port, port number range: 1024 - 65535')
        sys.exit()
    return port

def serverLoop(sock, port, user_handle):
    """
    Function ~ serverLoop
    Parameters ~ socket file descriptor, port number, user's handle
    Preconditions ~ passed an open socket, bound port number and user handle
    Postconditions ~ server chatroom is run and stays running until CTRL+C
    Description ~ accepts a new connection and sends that connection off to 
    theLoop for processing.  After the loop breaks, closed that connection
    and goes back to listening for a new connection.
    Prints some informational messages
    """

    columns = getTerminalCols()

    while 1:
        print ('Server listening on: ', socket.gethostname(), ' port: ', port)
        conn, addr = sock.accept()
        print ('Connected by ', addr)
        theLoop(conn, user_handle)
        conn.close()
        print ('Closed connection to ', addr)

def clientLoop(sock, user_handle):
    """
    Function ~ clientLoop
    Parameters ~ socket file descriptor, user's handle
    Preconditions ~ valid connected socket to server, string for user handle
    Postconditions ~ client communicates with the server
    Description ~ send an initial message that include the local port number
    this client is using to receive, then enters theLoop with the socket to 
    communicate with the server. when theLoop exits this function closes the connection
    """
    port = sock.getsockname()[1]
    send_text(sock, user_handle + CARET + str(port) + '\n')
    theLoop(sock, user_handle)
    sock.close()

def theLoop(sock, user_handle):
    """
    Function ~ theLoop
    Parameters ~ socket file descriptor, user_handle string
    Preconditions ~ passed connected socket and string for user_handle
    Postconditions ~ sends and receives messages until \quit is found
    Description ~ using select() checkes whether there is data from stdin and if
    so then get's that data and passes it to send_text to send it to the connected
    host.  Other wise it checks the socket to see if there is data to be received
    if at any point either host sends \quit it breaks this loop and returns to the
    calling function
    """
    columns = getTerminalCols()
    run = 1
    print (user_handle+CARET, end='')
    sys.stdout.flush()
    while(run):
        readables, writeables, exceptready = select.select([sock, sys.stdin],[],[], 0.1)
        for s in readables:
            if s == sys.stdin:
                reply = get_input(user_handle)
                send_text(sock, reply)
                if check_for_quit(reply) == 1:
                    print ('Closing connection\n')
                    run = 0
                    break
                reply = ''
                print (user_handle+CARET, end="")
                sys.stdout.flush()
            else:
                message = recv_all(s)
                if check_for_quit(message) == 1:
                    print ('Connection closed by remote\n')
                    run = 0
                    break
                print_message(message, columns, user_handle)
                
def getTerminalCols():
    """
    Function ~ getTerminalCols
    Parameters ~ none
    Preconditions ~ none
    Postconditions ~ returns int
    Description ~ returns the columns of the terminal window size as an integer
    """

    rows,columns = os.popen('stty size', 'r').read().split()
    return int(columns)


def print_message(message, cols, user_handle):
    """
    Function ~ print_message
    Parameters ~ string, int, string
    Preconditions ~ pass valida parameters, cols should be the size of terminal window
    Postconditions ~ message printed to screen
    Description ~ iterates through the message by half the number of columns in the 
    terminal window, right pads a enough spaces to get the actual message printed to
    the right hand side of the window.
    Finally after the incoming message has been printed also prints the user_handle
    and caret to signify the user can input data again
    Cite: 
    https://stackoverflow.com/questions/566746/how-to-get-linux-console-window-width-in-python
    """
    #if '\n' not in message:
     #   message += '\n'

    space = ' '

    print ("\n", end="")
    #sys.stdout.flush()

    for i in range(0,len(message), int(cols/2)):
        if len(message) < int(cols/2):
            print (space.rjust(int(cols/2)-4), " * ", message[i:i+int(cols/2)], end="")
        else:
            print (space.rjust(int(cols/2)-4), " * ", message[i:i+int(cols/2)])
    print (user_handle+CARET, end="")
    sys.stdout.flush()


def send_text(conn, text):
    """
    Function ~ send_text
    Parameters ~ socket file descriptor, string
    Preconditions ~ open socket and valid string
    Postconditions ~ message sent to host
    Description ~ appends the string with the length of the actual payload, converts
    the int to a host to network short.  Pads the first five chars with spaces 
    in case the digits are shorter.  The other host will read 5 bytes worth. 
    Then appends the message and sends it all
    """
    msg = str(socket.htons(len(text)))
    msg = msg.ljust(5)
    msg += text
    try:
        conn.sendall(bytes(msg, 'UTF-8'))
    except TypeError:
        conn.sendall(str(msg))

def get_input(user_handle):
    """
    Function ~ get_input
    Parameters ~ string
    Preconditions ~ user_handle as string
    Postconditions ~ line from stdin returned
    Description ~ reads in the line from stdin up to a newline, shortens the message to
    the MAX_MESSAGE length, appends a newline if needed.  the returns the string
    with the user_handle and CARET prepended.
    """

    reply = sys.stdin.readline()
    reply = reply[:MAX_MESSAGE]
    if not '\n' in reply:
        reply += '\n'
    return user_handle + CARET + reply

def recv_all(conn):
    """
    Function ~ recv_all
    Parameters ~ socket file descriptor
    Preconditions ~ open socket
    Postconditions ~ returns the entire message 
    Description ~ reads 5 bytes of the packet, the first 5 bytes are expected to contain
    a network short integer for the number of bytes in the remaining payload. Converts
    the network short to a host integer.
    Then passes this length with the socket to the recv_msg function to receive the
    entire message.
    Cite ~ https://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data
    """
    try:
        raw_msglen = recv_msg(conn,5)
        if not raw_msglen:
            return None
        raw_msglen = re.sub("\D", "", raw_msglen)
        msglen = socket.ntohs(int(raw_msglen))
        return recv_msg(conn, msglen)
    except socket.error:
        pass

def recv_msg(conn, msglen):
    """
    Function ~ recv_msg
    Parameters ~ socket file descriptor, length
    Preconditions ~ open socket, length is bytes to receive
    Postconditions ~ data returned
    Description ~ for the number of bytes in msglen, loops on receiving from the socket
    until all the expected bytes have been received.  then returns the data that
    was received.
    """
    data = ''
    message = ''
    bytesread = 0

    try:
        while bytesread < msglen:
            data = conn.recv(msglen - bytesread)
            if not data:
                break
            bytesread += len(data)
            message += str(data.decode())
    
        return message
    except socket.error:
        return ''

def check_for_quit(message):
    """
    Function ~ check_for_quit
    Parameters ~ string, string
    Preconditions ~ first string is the message
    Postconditions ~ returns 1 on \quit found else returns 0
    Description ~ finds the first location in the string of the words \quit
    if that location is immediately after the length of the user_handle and the next
    five characters match \quit, we return 1, to signal that we want to close the 
    connection
    """
    command = ''

    try:
        index = message.find(QUIT)
        index2 = message.find(CARET)

        if index != -1 and index < index2 + 3 and message[index:index+5] == QUIT:
            return 1
        else:
            return 0
    except AttributeError:
        return 1

def initServer(port):
    """
    Function ~ initServer 
    Parameters ~ port number
    Preconditions ~ valid port number 
    Postconditions ~ returns a bound socket file descriptor
    Description ~ prepares the socket as a TCP socket, then sets the sockopt for
    reuse so the OS doesn't hold on to it after the program quits, tries to bind the
    port number and localhost to the socket. then begings listening on the socket
    returns the listening socket.
    """
    host = ''
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error:
        print ('Failed to create socket')
        sys.exit()

    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    try:
        sock.bind((host, port))
    except socket.error:
        print ('Failed to bind socket')
        sys.ext()

    sock.listen(1)
    return sock

def initClient(host, port):
    """
    Function ~ initClient
    Parameters ~ hostname, port number
    Preconditions ~ valid host name and port number
    Postconditions ~ returns a socket file descriptor
    Description ~ prepares the socket as a TCP socket, attempts to connection to
    the host and port number passed in, if successful returns the socket file
    descriptor, otherwise exits.
    """
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect((host, port))
    except socket.error:
        print ('Could not connect to the server')
        sys.exit()
    return sock

def getHandle():
    """
    Function ~ getHandle
    Parameters ~ none
    Preconditions ~ none
    Postconditions ~ returns string
    Description ~ requests a handle from the user, strips any newline characters
    found and then sets the handle to the max handle length
    """
    var = input('Please enter a handle (Max 10 char): ')
    var.rstrip('\n')
    return var[:MAX_HANDLE]

def number(num):
    """attempts to cast the variable to a number otherwise returns -1"""
    try:
        return int(num)
    except ValueError:
        return -1

if __name__ == "__main__":
    main(sys.argv)
