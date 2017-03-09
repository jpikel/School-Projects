"""Filename: coinchange
    Authors: Kelsey Helms, Jay Steingold, Johannes Pikel
    Date: 2016.10.18
    Class: CS325-400
    Assignment: Project 2
    Python version: script written for Python version 2.7.5
"""

from __future__ import print_function
import sys
import time
#import os
#import random
#import math
#from changedp import changedp
#from changegreedy import changegreedy
#from changeslow import changeslow

#check Python version
if sys.version_info < (2, 7):
    input("Your Python interpreter may be too old, please upgrade to at least 2.7.5")
    sys.exit()

#set default timer to use for timing algorithms
if sys.platform == "win32":
    DEFAULT_TIMER = time.clock
else:
    DEFAULT_TIMER = time.time


def create_test_file():
    """Function: create_test_file
    Description: create test files for the problems 3 through 5 with arrays and values of A
    as a .txt file for input into the program
    Parameters:
    Preconditions:
    Postconditions:
    """

def read_in_file(file_name):
    """Function: read_in_file()
    Description: using the file_name passed attempts to open the file for reading.  the file should
    be formated as
    [1, 5, 10, 25, 50] //denominations that may be used
    75                  //change desired
    Returns an array in which there are multi-integer arrays that are the denominations followed by
    a single element array that is the change to be made
    Parameters: file_name as string
    Preconditions: text file comma delimited in the following format
    [1,2,3,...,] //is the denominations
    [1]         //is the change to be made
    Postconditions: returns list_of_lists contains lines from file
    Cite: http://stackoverflow.com/questions/18304835/parsing-a-text-file-into-a-list-in-python
    """
    list_of_lists = []

    try:
        with open(file_name) as file_handle:
            for line in file_handle:
                line = line.strip('[')              #strip [
                line = line.strip()                 #strip CR/LF
                line = line.strip(']')              #strip ]
                line = line.split(',')              #separate the line by commas
                try:                                #skip lines that do not contain integers
                    line = [int(x) for x in line]   #convert the string list into integer list
                    list_of_lists.append(line)
                except ValueError:
                    pass
        return list_of_lists

    except IOError:
        print("\nError Opening File.\n")
        return -1

#this also needs updating for the new project!!
def write_to_file(change_result, min_num_coins, file_name):
    """Function: write_to_file
    Description: appends to file the array of ints that is the denominations used and then
    the minimum number of coins used in the following format
    [0, 1, 2]
    3
    Parameters: array of ints, int, file_name as string
    Preconditions: arguments passed otherwise nothing will be written
    Postconditions: data written to file
    """
    file_handle = open(file_name, "a") #"a" is for append
    line = "["
    for item in change_result:
        line += str(item) + ", "
    line = line[:-2]
    line += "]\n" + str(min_num_coins) + "\n"
    file_handle.write(line)
    file_handle.close()

def write_to_comma_delim(min_num_coins, file_name):
    """writes only min_num_coins for a test set in comma delimitated format for quick import to excel"""
    file_name = file_name[:-4]
    file_name = file_name + "_comma.txt"
    file_handle = open(file_name, "w")
    line = ""
    for item in min_num_coins:
        line += str(item) + ", "
    line = line[:-2]
    file_handle.write(line)
    file_handle.close()

def write_to_runtime(run_time, file_name):
    """writes an array of runtimes to a comma delimited file for import to excel"""
    file_name = file_name[:-4]
    file_name = file_name + "_runtime.txt"
    file_handle = open(file_name, "w")
    line = ""
    for item in run_time:
        line += str(item) + ", "
    line = line[:-2]
    file_handle.write(line)
    file_handle.close()


def run_this(function, function_prefix, full_file_name):
    """Function: run_this
    Description: used to run the various algorthims to find the minimum number of coins
    required to make change.
    Using the passed function, the file's prefix as in the function name and the
    full file name as in "Coin1.txt"
    Times the function per array it processes and prints the times to the screen for
    recording.
    Parameters: function, string function name, string full_file_name
    Preconditions: passed a valid function name, and file name
    Postconditions: data written to file (coins used and minimum number of coins)
    """
    short_file_name = full_file_name
    short_file_name = short_file_name[:-4] #remove the .txt from the end of the full_file_name
    list_of_lists = read_in_file(full_file_name)
    if list_of_lists != -1:
        print('\n')
        result_file_name = short_file_name + "change.txt" #write the file_name prefix + "change" to the file
        with open(result_file_name, 'a') as file_handle:
            line = "Algorithm " + function_prefix + "\n"
            file_handle.write(line)
            if len(list_of_lists) == 0: #if list_of_lists failed to return anything
                file_handle.write(function_prefix + " ")
                file_handle.write("Nothing in the list!\n")
                file_handle.close()
            else:
                file_handle.close() #close from writing the function name to file
                index_print = 1
                num_arrays = 0 #new
                total_time = 0 #new
                min_num_coins_array = [] #used to write comma delimitated file #comment out later
                run_time_array = [] #comment out later
                for index in range(0, len(list_of_lists), 2): # this for loop iterates over 2 items each iteration
                    num_arrays += 1 #new
                    denominations = list_of_lists[index]
                    change_to_make = list_of_lists[index + 1]
                    change_to_make = int(change_to_make[0]) #convert change_to_make list into int

                    time_0 = DEFAULT_TIMER()
                    change_result, min_num_coins = function(denominations, change_to_make) #run the function passed with the
                    time_1 = DEFAULT_TIMER()

                    #print(change_result)#for testing
                    #print(min_num_coins)#for testing
                    print("Array ", index_print, " time: ", (time_1 - time_0)) #show the time difference
                    total_time += (time_1 - time_0) #new
                    index += 1
                    write_to_file(change_result, min_num_coins, result_file_name)

                    #comment these lines out when finished gathering data
                    run_time_array.append(total_time)
                    min_num_coins_array.append(min_num_coins)
                write_to_comma_delim(min_num_coins_array, result_file_name)
                write_to_runtime(run_time_array, result_file_name)
                #stop commenting out after data gathered..

        print("\n", function_prefix, " results written to file\n\n")

        print("The average time for " + function_prefix + " was: " + str(total_time / num_arrays) + "\n\n")
