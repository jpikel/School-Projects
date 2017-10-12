"""
Filename: pikelj_ftclient.py
Author: Johannes Pikel
Date: 2017.08.02
Class: CS372-400
Assignment: Project #2
Description: this runs the client that connects to a server for the file transfer
program.  Opens a local server that is the data socket and waits for the transmitting
of actual data.  All other commands are received and sent via the command socket
Cite: General reference used from this site:
    https://docs.python.org/2/library/socket.html
"""

from __future__ import print_function
import socket   #gives us access to the connections
import sys      #for access to the commandline arguments
import os       #so we have access to the terminal size
import os.path
try:
    import __builtin__ # py 2 and 3 input compatible
    input = getattr(__builtin__, 'raw_input')
except (ImportError, AttributeError):
    pass
import re       #strip away the letters
import platform
import select
import fcntl
import struct

def main(argv):
    """
    Function ~ main()
    Parameters ~ command line
    Preconditions ~ passed in {hostname} {hostport} {command} [filename] {localport}
    Postconditions ~ connection made to file transfer server and command sent
    Description ~  validates the arguments, attempts to connect to the server on the
    hostport / command socket, starts the local client server with the localport and
    data socket.  then makes the request to the server.  finally closes both sockets

    """
    serverport, localport = valArguments(argv)
    #initialize up the local data port, that acts like a local server
    commandsock = initiateContact(argv[1], serverport)
    datasock = initServer(localport)
    #make the request to the server
    make_request(argv, commandsock, datasock, localport)
    datasock.close()
    commandsock.close()

    return 0


def valArguments(argv):
    """
    Function ~ valArguments
    Parameters ~ command line arguments
    Preconditions ~ passed in command line argument
    Postconditions ~ returns the server connection port and the local data port
    Description ~  if we do not have either 5 or 6 command line arguments (1 is the
    executable, then we have not passed in the correct number of argument so exit with
    a usage statement
    the host port always is in index 2, check that it is a number and in range
    the local data port can be in position 4 or 5 depending on the other arguments
    validate that it is a number and in range
    if we passed in the -g command we need to also pass in a filename if we didn't then
    exit the system
    """
    usage = 'Usage: $ python pikelj_ftclient.py {hostname} {hostport}'
    usage += ' {command} [filename] {clientportnumber}'   
    if not (len(argv) == 5 or len(argv) == 6):
        print (usage)
        sys.exit()

    if argv[2].isdigit():
        port = number(argv[2])
        checkrange(port)
    else:
        print ('Server port number is not a valid digit')
        sys.exit()

    if (len(argv) == 5 and argv[4].isdigit()) or (len(argv) == 6 and argv[5].isdigit()):
        x = 5
        if(len(argv) == 5):
            x = 4
        localport = number(argv[x])
        checkrange(localport)
    else:
        print('Local port number is not a valid digit')
        sys.exit()

    if argv[3] == "-g" and len(argv) == 5:
        print('If passing argument -g must also pass file name')
        print (usage)
        sys.exit()

    if argv[3] == "-l" and not len(argv) == 5:
        print('Invalid number of arguments')
        sys.exit()
    return port, localport

def checkrange(port):
    """
    Function ~ checkrange
    Parameters ~ port number
    Preconditions ~ passed number
    Postconditions ~ exists the system if check fails
    Description ~ if the port number passed in is not in the usuable range
    print a message and exit the program
    """
    if port < 1024 or port > 65535:
        print ('Invalid Port, port number range: 1024 - 65535')
        sys.exit()

def make_request(argv, commandsock, datasock, localport):
    """
    Function ~ make_request
    Parameters ~ command line arguments, command socket, data socket, local data port
    Preconditions ~  valid sockets passed in
    Postconditions ~ request made and retrieved from server
    Description ~ depending on the command passed in from the command line, if we passed
    in -l then we make the request to the server to retrieve the directory listing
    if it's -g we should also have passed in a file name, request that file from the
    server and if it exists retrieve the file.
    Otherwise print the message from the server, which should be invalid command
    """
    #request the files in the folder
    if argv[3] == "-l":
        send_text(commandsock, (argv[3] + ' ' + argv[4]))
        conn, addr = datasock.accept()
        print('Receiving directory structure from', argv[1], ":", localport, '\n')
        message = recv_all(conn)
        print(message)
        conn.close()
    #request a file here with the -g command
    elif argv[3] == "-g":
        send_text(commandsock, (argv[3] + ' ' + argv[4] + ' ' + argv[5]))
        fsize = recv_all(commandsock)
        #if the file does not exist we get back a -1
        if number(fsize) > 0:
            #check if we already have the file on our local host, rename as needed
            filename = check_if_file_exists(argv[4])
            #if the user aborts we send the abort message to the server
            #no file contents should be transmitted
            if filename == -1:
                send_text(commandsock, "ABORT")
            else:
                #otherwise we are ready and can accept data on the data socket
                send_text(commandsock, "READY")
                conn, addr = datasock.accept()
                print('Receiving \"', filename, '\" from', argv[1], ":", localport, '\n')
                receive_file(conn, filename, fsize)
                print('Transfer complete. File \"', filename, '\" received.\n')
                conn.close()
        else:
            #if we got a -1 from the server it does not exist on the server
            print(argv[1],":",argv[5], " says FILE NOT FOUND")
    else:
        #all other commands at this time are not implemented
        send_text(commandsock, (argv[3]))
        reply = recv_all(commandsock)
        print(argv[1], "says", reply)

def check_if_file_exists(filename):
    """
    Function ~ check_if_file_exists
    Parameters ~  string
    Preconditions ~ passed string
    Postconditions ~ returns string of filename
    Description ~ Check if the file exists.  If it does give the user the option
    to overwrite or rename it.  If renaming we need to check again that the new
    filename does not exist.  Continue until we either have a unique filename or
    we overwrite, then return the filename.
    """
    if os.path.isfile(filename):
        print('File already exists overwrite y/n or abort')
        answer = input('? ')
        if answer == 'y' or answer == 'Y' or answer == 'Yes' or answer == 'yes':
            print('Overwriting file.')
        elif answer == 'abort':
            filename = -1
        else:
            filename = input('Please enter a new name: ')
            check_if_file_exists(filename)
    return filename

def receive_file(conn, filename, filesize):
    """
    Function ~ receive_file
    Parameters ~  int, string, int
    Preconditions ~ passed a socket with connection, filename, and filesize
    Postconditions ~ file received
    Description ~ continues to receive messages from the server until all the
    bytes we've read meet the filesize.  Then we know the file contains all the 
    bytes from the original. For each message we receive we write that message out
    to the file.  Once done we close the file
    """

    total = 0
    fp = open(filename, 'wb')

    while total < filesize:
        msg = recv_all(conn)
        try:
            fp.write(msg)
            total += len(msg)
        except TypeError:
            fp.close()
            break

    fp.close()

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
            message += data
    
        return message
    except socket.error:
        print('socket error')
        return ''

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
        print ('Server app failed to create socket')
        sys.exit()

    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    try:
        sock.bind((host, port))
    except socket.error:
        print ('Server app failed to bind socket: ', port)
        sys.exit()

    sock.listen(1)
    return sock

def initiateContact(host, port):
    """
    Function ~ initiateContact
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
        print ('Connected to: ', host, ' on: ', port)
    except socket.error:
        print ('Could not connect to the server')
        sys.exit()
    return sock

def number(num):
    """attempts to cast the variable to a number otherwise returns -1"""
    try:
        return int(num)
    except ValueError:
        return -1

if __name__ == "__main__":
    main(sys.argv)
