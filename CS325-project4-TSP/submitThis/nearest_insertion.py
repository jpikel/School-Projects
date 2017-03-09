"""Filename: nearest_insertion.py
Description: Basic implementation of the nearest neighbor algorithm
Assignment: Project 4
Group 6: Kelsey Helms, Jay Steingold, Johannes Pikel
Class: CS325 - 400
Date: 2016.11.23
Due Date: 2016.12.02
"""

from __future__ import print_function
import math
import sys
import time

#set default timer to use for timing algorithms
if sys.platform == "win32":
    DEFAULT_TIMER = time.clock
else:
    DEFAULT_TIMER = time.time

def read_in_file(FILE_NAME):
    """Function: read_in_file()
    Description: 
    Parameters: file_name as string
    Preconditions: text file white space delimited in the following format
    int int int\n
    Postconditions: returns list_of_lists contains lines from file
    Cite: http://stackoverflow.com/questions/18304835/parsing-a-text-file-into-a-list-in-python
    """
    list_of_cities = []

    try:
        with open(FILE_NAME) as file_handle:
            for line in file_handle:
                line = line.strip('\n')                 #remove CR/LF
                line = line.split()                     #remove any white space
                #print                        #for testing
                try:                                #skip lines that do not contain integers
                    line = [int(x) for x in line]   #convert the string list into integer list
                    list_of_cities.append(line)
                except ValueError:
                    pass
        return list_of_cities
    except IOError:
        print("\nError Opening File.\n")
        return -1

#Get the distance between two city points
#From tsp-verifier.py
def getCityDistance(a, b):
    # a and b are integer pairs (each representing a point in a 2D, integer grid)
    # Euclidean distance rounded to the nearest integer:
    dx = a[1]-b[1]
    dy = a[2]-b[2]
    #return int(math.sqrt(dx*dx + dy*dy)+0.5) # equivalent to the next line
    return int(round(math.sqrt(dx*dx + dy*dy)))

def nearest_insertion(FILE_NAME):
    """implementation of the nearest insertion algorithm"""
    time_0 = DEFAULT_TIMER()
    totalDistance = 0    
    list_of_cities = read_in_file(FILE_NAME)
    #start of the partial tour
    tour = [list_of_cities[0], list_of_cities[0]] 
    list_of_cities.remove(list_of_cities[0])
    #Place each city in the partial tour where it would increase the total weight the least
    for city in list_of_cities:
        minDistance = sys.maxint - 1
        minCity = tour[0]
        #Check to see where it can be added to increase the weight the least
        for i in range(0, len(tour) - 2):
            d1 = getCityDistance(city, tour[i])
            d2 = getCityDistance(city, tour[i + 1])
            if(d1 + d2 < minDistance):
                minCity = tour[i]
                minDistance = d1 + d2         
               
        #split the tour at the index of the smaller city
        firstHalf  = tour[:tour.index(minCity) + 1]
        secondHalf = tour[tour.index(minCity) + 1:]
        #Add new city to the middle of the halves and recombine
        tour = firstHalf + [city] + secondHalf     
    #When the tour is finished  
    #get the distance
    for i in range(0, len(tour) - 1):
        d = getCityDistance(tour[i], tour[i + 1])
        totalDistance += d

    #Remove the final city from the tour
    tour.pop()

    #get the cities
    cities = []
    for city in tour:
        cities.append(city[0])
    
    time_1 = DEFAULT_TIMER()
    print("Time taken: ", (time_1 - time_0)) 
    write_to_file(cities, int(totalDistance), FILE_NAME)

def write_to_file(cities, distance, FILE_NAME):
    """write to file"""
    FILE_NAME += ".tour"
    file_handle = open(FILE_NAME, "w")
    file_handle.write(str(distance) + "\n")
    for city in cities:
        file_handle.write(str(city) + "\n")
    file_handle.close()

class Node(object):
 
    def __init__(self, data, prev, next):
        self.data = data
        self.prev = prev
        self.next = next
 

nearest_insertion(sys.argv[-1])
