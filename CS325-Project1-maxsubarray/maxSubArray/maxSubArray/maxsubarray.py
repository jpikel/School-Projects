"""Filename: maxsubarray
    Authors: Kelsey Helms, Jay Steingold, Johannes Pikel
    Date: 2016.10.06
    Class: CS325-400
    Assignment: Project 1
    Python version: script written for Python version 2.7.5
"""
from __future__ import print_function
import sys
import time
import os
import random
import math

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
    """Function: create_test_file()
    Description: creates a .txt file with 10 comma delimited arrays in the format [1,2,3,..,n] with
    n elements, n elements are input by the user
    Parameters: none
    Preconditions: none
    Postconditions: .txt file generated
    """
    n_elements = raw_input("\nPlease enter the number of n elements per array: ")
    try:
        n_elements = int(n_elements)
        n_elements = abs(n_elements) #make sure only positive integer
        file_name = str(n_elements) + ".txt"
        file_handle = open(file_name, 'w')
        random.seed()
        for i in range(10): #changed from 11
            line = "["
            for x in range(0, n_elements):
                line += str(random.randint(-100, 100)) + ", "
            line = line[:-2]
            line += "]\n"
            file_handle.write(line)
        file_handle.close()
        print("\n {}.txt created.\n".format(n_elements))
    except ValueError:
        print("\nNot a valid integer.\n")


def read_in_file():
    """Function: read_in_file()
    Description: Allows the user to enter a filename and attempts to read lines from that file into
    a list of lists.
    Parameters: none
    Preconditions: text file comma delimited in the format [1,2,3,...,]
    Postconditions: list_of_lists contains lines from file
    Cite: http://stackoverflow.com/questions/18304835/parsing-a-text-file-into-a-list-in-python
    """
    list_of_lists = []
    file_name = raw_input("Please enter the filename with extension," +
                          " leave blank to default to MSS_Problems.txt: ") #before submitting we'll comment out this line
    if file_name == "":
        file_name = "MSS_Problems.txt"

    #file_name = "MSS_Problems.txt" #and uncomment this line
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



def write_to_file(orig_array, max_array, max_sum):
    """Function: write_to_file
    Description: appends to file the original array, the max sub array and the max sum in
    the following format:
    [1, 2, 3, ...n n]
    [2, 3, ..., n]
    42
    Parameters: array of ints, array of ints, int
    Preconditions: arguments passed otherwise nothing will be written
    Postconditions: data written to file
    """
    file_handle = open("MSS_Results.txt", "a") #"a" is for append
    if max_sum == "no max sum":
        line = "Array of 0 length, no max sum.\n"
    else:
        line = "["
        for item in orig_array:
            line += str(item) + ", "
        line = line[:-2]
        line += "]\n["
        for item in max_array:
            line += str(item) + ", "
        line = line[:-2]
        line += "]\n" + str(max_sum) + "\n\n"
        file_handle.write(line)
    file_handle.close()


def enumeration(array):
    """Function: enumeration
    Description: Looping over each pair of indices i, j and compute the sum.  check if the new sum
    larger than the previous max sum, if so keep it.  Also keep the first index and last index so
    the max sub array may be stored.
    Parameters: array of integers
    Preconditions: array of ints passed
    Postconditions: returns max sum and max sum array
    Cite: http://stackoverflow.com/questions/33182826/understanding-implementing-this-enumeration-solution-for-maximum-subarray
    """
    max_sum = new_max_sum = -sys.maxint
    low_index = high_index = 0
    if len(array) >= 1:
        for outer_index in range(0, len(array)+1):
            for inner_index in range(outer_index+1, len(array)+1):
                new_max_sum = sum(array[outer_index:inner_index])

                if new_max_sum > max_sum:
                    max_sum = new_max_sum
                    low_index = outer_index
                    high_index = inner_index
        return array[low_index:high_index], max_sum
    else:
        max_sum = "no max sum"
        return array, max_sum


def iteration(array): #was called enumeration but I mistakenly implemented iteration here jp
    """Function: iteration
    Description: This function scans through an array to find the maximum sub array and it's sum
    It does this by iterating through each index, adding the remaining indices to it and checking
    if it is the largest sum so far.
    Parameters: array of integers
    Preconditions: array passed to function
    Postconditions: returns the max_sum and max sum array
    """
    max_sum = new_max_sum = -sys.maxint
    low_index = high_index = 0
    if len(array) > 1:
        for outer_index in range(0, len(array)+1):
            new_max_sum = 0
            for inner_index in range(outer_index, len(array)):
                new_max_sum += array[inner_index]
                if new_max_sum > max_sum:
                    low_index = outer_index
                    high_index = inner_index
                    max_sum = new_max_sum

        return array[low_index:high_index + 1], max_sum
    elif len(array) == 1:
        max_sum = array[0]
        return array, max_sum
    else:
        max_sum = "no max sum"
        return array, max_sum

def max_crossing_subarray(array, low, mid, high):
    """Function: max_crossing_subarray
    Description: Helper function to the divide and conquer method of finding a
    max sub array.  This function iteratively finds the maximum subarray starting
    from the mid-point in the range from the low to high index.
    Parameters: array of int, int, int, int
    Preconditions: passed array of ints and, low, mid and high indices contained in the array
    Postconditions: returns the low index and high index, plus the max sum
    Cite: "Introduction to Algorithms" page 71, Cormen, Thomas H.; Leiserson, Charles E.;
    Ronald L.; Stein, Clifford; The MIT Press
    """
    right_sum = left_sum = -sys.maxint
    sum = 0
    max_left = max_right = -1
    for index in range(mid, low-1, -1):
        sum += array[index]
        if sum > left_sum:
            left_sum = sum
            max_left = index

    sum = 0
    for index in range(mid+1, high+1):
        sum += array[index]
        if sum > right_sum:
            right_sum = sum
            max_right = index

    return max_left, max_right, (left_sum + right_sum)

def find_maximum_subarray(array, low, high):
    """Function:find_maximum_subarray
    Description: This function divides the array in half searching each subarray for
    the max sub array.  It also searched the suffix and prefix of the divided halves
    to check if the max sub array cross the divide. Uses max_crossing_subarray as helper
    function to find the max subarray that crosses the divided halves.
    Parameters: array of int, low index, high index
    Preconditions: passed an array of ints, low is array's lowest index, high is
    arary's highest index
    Postconditions: returns low index, high index and max sum of max subarray
    Cite: "Introduction to Algorithms" page 72, Cormen, Thomas H.; Leiserson, Charles E.;
    Ronald L.; Stein, Clifford; The MIT Press
    """
    if high == low:
        return low, high, array[low]
    else:
        mid = int(math.floor((low + high)/2))
        left_low, left_high, left_sum = find_maximum_subarray(array, low, mid)
        right_low, right_high, right_sum = find_maximum_subarray(array, mid + 1, high)
        cross_low, cross_high, cross_sum = max_crossing_subarray(array, low, mid, high)

        if left_sum >= right_sum and left_sum >= cross_sum:
            return left_low, left_high, left_sum
        elif right_sum >= left_sum and right_sum >= cross_sum:
            return right_low, right_high, right_sum
        else:
            return cross_low, cross_high, cross_sum

def recursive(array):
    """Function: Helper function used by run_this() in order to properly start
    the recursive find_maximum_subarray()
    Description: stores the low, high index and max sum of the max subarray
    and returns the array[low:high] range and max sum to run_this so it may be written
    correctly into the MSS_results.txt file
    Parameters: array of ints
    Preconditions: an array of ints must be passed in
    Postconditions: returns maximum subarray and max_sum of that subarray
    """
    low, high, sum = find_maximum_subarray(array, 0, len(array)-1)
    return array[low:high + 1], sum


def recursion_inversion(array):
    """Function: Dynamic Programming solution to the maximum subarray problem.
    This function iterates over the array, seeing if the the current array item
    increases the current maximum and if that current maximum is greater than the
    highest maximum.
    Parameters: An array of ints
    Preconditions: Must have an array of ints passed in
    Postconditions: Returns the sum of the maximum subarray
    """

    if len(array) == 0:
        return array, "no max sum"

    maxMax = currentMax = array[0]
    idx = 0
    low_idx = high_idx = 0


    for index in range(1, len(array)):
        if array[index] > (currentMax + array[index]):
           currentMax = array[index]
           idx = index
        else:
            currentMax += array[index]

        if currentMax > maxMax:
            maxMax = currentMax
            low_idx = idx
            high_idx = index

    return array[low_idx:high_idx+1], maxMax



"""    currentArr = []
    maxArr = []
    currentMax = -sys.maxint #array[0]
    maxMax = -sys.maxint #array[0]

    for i in range (0, len(array)):
       # currentMax = max(array[i], currentMax + array[i])
       # maxMax = max(maxMax, currentMax)

    #return newArray, maxMax

        #Set currentMax
        if array[i] > currentMax + array[i]:
            currentMax = array[i]
            currentArr = []
            currentArr.append(array[i])
        
        elif array[i] <= currentMax + array[i]:
            currentMax = currentMax + array[i]
           # if(currentMax + array[i] > maxMax):
            currentArr.append(array[i])

        #Set maxMax
        if maxMax < currentMax:
            maxMax = currentMax
            maxArr = currentArr
    print(maxArr)
    return maxArr, maxMax"""


def delete_old_results():
    """Function: delete_old_results()
    Description: asks user if they wish to delete the old results.txt file
    Parameters: none
    Preconditions: na
    Postconditions: delete file if requested
    """
    selection = 'n'
    while selection == 'n':
        selection = raw_input("Do you want to remove the old MSS_results.txt file? (y/n)\n")
        if selection == 'y' or selection == 'Y' or selection == 'yes':
            try:
                os.remove("MSS_results.txt")
                print("\nFile removed.\n")
            except OSError:
                print("\nNo file found.\n")
        elif selection == 'n' or selection == 'N' or selection == 'no':
            selection = 'q'
        else:
            selection = 'n'

def run_this(function, name):
    """Function: run_this
    Description: used to run the various algorthims to find the max sub array.
    Using the passed function and it's name so it can be written to file.
    Also times the function per array it processes and prints the times to the screen for
    recording.
    Parameters: function, string name
    Preconditions: passed a valid function name
    Postconditions: data written to file (max sub array and max sum)
    """
    list_of_lists = read_in_file()
    if list_of_lists != -1:



        """print('\n')
        with open("MSS_Results.txt", 'a') as file_handle:
            name += "\n"
            file_handle.write(name)
            if len(list_of_lists) == 0:
                file_handle.write("Nothing in the list!\n")
            file_handle.close()
            index = 1
            numArrays = 0 #new
            totalTime = 0 #new
        for item in list_of_lists:
            numArrays += 1 #new
            time_0 = DEFAULT_TIMER()
            this_array, max_sum = function(item)
            time_1 = DEFAULT_TIMER()
            print("Array ", index, " time: ", (time_1 - time_0))
            totalTime += (time_1 - time_0) #new
            index += 1
            write_to_file(item, this_array, max_sum) 
        file_handle.close()
        name = name.strip('\n')
        print("\n", name, " results written to file\n\n")
        print("The average time for " + name + " was: " + str(totalTime / numArrays) + "\n\n")"""
        


def menu():
    """Function: menu()
    Description: runs a menu to select what type of maxSubArray function to run
    Parameters: none
    Preconditions: none
    Postconditions: menu displayed on screen
    """

    print("Welcome...\n\n")

    delete_old_results()

    condition = True

    while condition:
        selection = raw_input("Select from these options to find the max sub-array: " +
                              "\n1. Enumeration" +
                              "\n2. Iteration" +
                              "\n3. Divide & conquer" +
                              "\n4. Recursion inversion" +
                              "\n5. Exit" +
                              "\n6. Create file with 10x test arrays of n elements." +
                              "\nYour entry: ")

        if selection == "1":                                        #enumeration
            run_this(enumeration, "Enumeration")
        elif selection == "2":                                      #iteration
            run_this(iteration, "Iteration")

        elif selection == "3":                                      #Divide & conquer
            run_this(recursive, "Divide and Conquer")
        elif selection == "4":
            run_this(recursion_inversion, "Recursion Inversion")    #Recursion inversion
            #condition = True

        elif selection == "5":                                      #exit
            condition = False

        elif selection == "6":                                      #create test file
            create_test_file()


menu()
