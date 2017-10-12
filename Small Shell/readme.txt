Filename: readme.txt
Author: Johannes Pikel
Date: 2016.02.20
ONID: pikelj
Class: CS340-400
Assignment: Program 3 smallsh
Description: readme file

Please copy these file to the same directory:
compileall
pikelj.smallsh.c
p3testscript

To compile into an executable program please run the following 
command from the bash shell

It may be necessary to add execute rights so please use 

$ chmod +x compileall

Then run

$ ./compileall

This should compile and link the program completely

Should this fail please use

$ gcc -x c -g -Wall -pedantic-errors pikelj.smallsh.c -o smallsh

If smallsh or p3testscript do not have execute rights for the user please
change it with 

chmod +x <filename>

Enter the bash prompt and from there 
you may run the test script to screen output with

$ ./p3testscript

or redirect to a file with

$ ./p3testscript > mytestresults 2>&1

If you would just like to enter the shell run

$ ./smallsh
