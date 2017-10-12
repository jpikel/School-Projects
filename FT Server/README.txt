Filename: README.txt
Author: Johannes Pikel
ONID: pikelj
Date: 2017.08.05
Class: CS372-400
Assignment: Project #2

I wrote the server in C and the client in python

Please unzip everything into the same folder, for best performance please move the
pikelj_ftclient.py into a separate folder.  Not a requirement but more notes on this 
below.

Please open two terminals and connect to the flip servers

In order to compile the C please just run the Makefile with
./make

First in order to start the server written in C in one terminal enter the following

./ftserver {portnumber}

port number should be a desired port number in the allowed user defined port range
I like using 50001

then in the other terminal start the client with
./python pikelj_ftclient.py {hostname} {hostportnumber} {command} [filename] {localdataportnumber}

If both terminals are connected to the same flip server the host port number and local
data port number must be different.  Regardless it is good practice to make these 
different number.

hostname is the hostname of the flip server where ftserver is running
hostportnumber is the portnumber you entered when starting the ftserver
command may be either -l or -g
    -l requests a listing of all the files in the current directory
    -g requests a file for transfer
filename is optional if you are no using -g, if you are using -g command then you must
    enter a filename
localdataport is the port number that the client will use to receive data from the server

If you run both client and server in the same folder, then when requesting a file for
transfer you will always get the message that the file exists, you do have an option to
overwrite the file or to rename it. I have tested overwriting the same file that exists
in the same directory and it seems to work just fine for small files, but I would expect
there to be some undefined behaviour.  Because we have two programs each reading and 
writing to the same file.

My recommendation but not necessary is to move the pikelj_ftclient.py into it's own
directory and run the ftserver and client from separate directories

I included a copy of alice.txt as an example of a short file to transfer

I also included a short bash scirpt called createfile, this takes alice.txt and appends
the contents into a new file called alice_big.txt 5000 times creating a text file
that is over 700 megabytes.



****Extra Credit****
my ftserver and client are also able to transmit binary files such as the ftserver
executable that you compiled with make

to check that the files are the same an easy command is ./diff -sq {file1} {file2}
