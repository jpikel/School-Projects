Filename: README.txt
Author: Johannes Pikel
ONID: pikelj
Date: 2017.07.18
Class: CS372-400
Assignment: Project #1

Please unzip all files to the same directory

I wrote the server in python and the client in C

Please open two terminals and connect to the flip servers

In order to compile the C please just run the Makefile with
./make

First in order to start the server written in python please enter in one terminal

./python pikelj_chat.py {portnumber}

port number should be a desired port number in the allowed user defined port range
I like using 50000

then in the other terminal start the client with
./chatroom {hostname} {portnumber}
So in our case because both terminals are connected to the same flip server you could
use ./chatroom localhost 50000

If your terminals are connected to two different flip servers such as flip2 and flip3
then after starting the server you will get a message that the server is listening
on, for instance flip2.engr.oregonstate.edu on port: 50000

When you connect with the client from another flip server you would need to enter
./chatroom flip2.engr.oregonstate.edu 50000

Then the server and client should be connected, there are error messages on bind, accept
and others. You should be able to pass messages back and forth 
between the client and server

To clean up the executable C file you can run ./make clean

Extra Credit: 
I also did a few extra credit parts for the assignment

1. 
I wrote the programs so that either can make contact first. For that to work you need to start the programs differently. So to start the C program as a server you would just do
./chatroom {portnumner}
and then to connect the python script to the host running C you would do
./python localhost {portnumber}

or whatever host you are running on

2.
You can send from either host at any time without taking turns
if you continued to enter into one terminal window the other host will continue to
receive.

3.
I split the screen, so the receiving message is printed on the right half of the terminal
and your input starts from the left side of the terminal.  Of course for really long
input it will continue across the terminal window
