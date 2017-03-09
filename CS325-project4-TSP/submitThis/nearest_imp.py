"""Filename: nearest_imp.py
Description: improved version of nearest neighbor that starts from the
pair of vertices that have the smallest edge.  Then does a double ended search
Assignment: Project 4
Group 6: Kelsey Helms, Jay Steingold, Johannes Pikel
Class: CS325 - 400
Date: 2016.11.22
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
    Description: reads in the FILE_Name passed.  Constructs a list of lists
    in the following format [(int, int,int, bool)] such that the bool is whether
    that city has been visited.
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
                #print(line)                        #for testing
                try:                                #skip lines that do not contain integers
                    line = [int(x) for x in line]   #convert the string list into integer list
                    line.append(False)              #add a False for visited to this particular city
                    list_of_cities.append(line)
                except ValueError:
                    pass
       # print(list_of_cities)
        return list_of_cities
    except IOError:
        print("\nError Opening File.\n")
        return -1

def nearest_neighbor(FILE_NAME):
    """implementation of the nearest neighbor algorithm
    improved such that the search now begins from the two vertices with the smallest
    distance between them and performs a double ended search. For the beginning and ending
    vertex of our solution we find the next optimal distance.  If the beginning vertex has a
    closest vertex than prepend our solution with the new vertex.  Otherwise the closest vertex
    is appended to the solution set."""
    time_0 = DEFAULT_TIMER()
    list_of_cities = read_in_file(FILE_NAME)

    #first find the two cities that are nearest each other and those will be
    #the two starting cities
    #list_of_cities_sorted = []
    nearest_dist = sys.maxsize

    for city_1 in list_of_cities:
        for city_2 in list_of_cities:
            if city_1[0] != city_2[0]:
                dx = city_1[1] - city_2[1]
                dy = city_1[2] - city_2[2]
                edge_length = int(round(math.sqrt((dx*dx) + (dy*dy))))
                #update nearest_city if the new distance is less than the current nearest distance
                if edge_length < nearest_dist:
                    nearest_dist = edge_length
                    starting_city = city_1
                    nearest_city = city_2

    #print(starting_city)
    #print(nearest_city)
    sol_of_cities = []                  #set our current solution to empty
    opt_distance = nearest_dist         #reset our current optimal solution
    nearest_dist = sys.maxsize          #nearest distance is huge
    starting_city[3] = True
    nearest_city[3] = True
    sol_of_cities.append(starting_city[0])
    sol_of_cities.append(nearest_city[0])

    city_1 = nearest_city
    city_3 = starting_city

    while len(sol_of_cities) < len(list_of_cities):         #check the current city to
                                                            #find the closest city in the
                                                            #list of cities not visited
            nearest_dist = sys.maxsize
            nearest_dist_2 = sys.maxsize
            for city_2 in list_of_cities:
                #if the two cities are not the same and have not visited the second city
                if city_1[0] != city_2[0] and city_2[3] != True:
                    dx = city_1[1] - city_2[1]
                    dy = city_1[2] - city_2[2]
                    dist_to = int(round(math.sqrt((dx*dx) + (dy*dy))))
                if city_3[0] != city_2[0] and city_2[3] != True:
                    dx_2 = city_3[1] - city_2[1]
                    dy_2 = city_3[2] - city_2[2]
                    dist_to_2 = int(round(math.sqrt((dx_2*dx_2) + (dy_2*dy_2))))
                #update nearest_city if the new distance is less than the current nearest distance
                    if dist_to < nearest_dist:
                        nearest_dist = dist_to
                        nearest_city = city_2
                    if dist_to_2 < nearest_dist_2:
                        nearest_dist_2 = dist_to_2
                        nearest_city_2 = city_2

            #add the city only if we haven't already visited it.
            if nearest_city[3] != True:
                if nearest_dist < nearest_dist_2:
                    sol_of_cities.append(nearest_city[0])
                    opt_distance += nearest_dist
                    nearest_city[3] = True
                    city_1 = nearest_city
                else:
                    sol_of_cities.insert(0, nearest_city_2[0])
                    opt_distance += nearest_dist_2
                    nearest_city_2[3] = True
                    city_3 = nearest_city_2

#add the distance from the last city we visited back to the first city to make a tour
    first_city = list_of_cities[sol_of_cities[0]]
    last_city = list_of_cities[sol_of_cities[len(sol_of_cities)-1]]
    dx = last_city[1] - first_city[1]
    dy = last_city[2] - first_city[2]
    opt_distance += int(round(math.sqrt((dx*dx) + (dy*dy))))
#if our current solution is better than the previous solution replace it, then start over until we've checked all cities

    #print(sol_of_cities)           #can remove later for testing now
    #print(opt_distance)            #same as above
    write_to_file(sol_of_cities, opt_distance, FILE_NAME)
    time_1 = DEFAULT_TIMER()
    print("Time taken: ", (time_1 - time_0))

def write_to_file(cities, distance, FILE_NAME):
    """write to file"""
    FILE_NAME += ".tour"
    file_handle = open(FILE_NAME, "w")
    file_handle.write(str(distance) + "\n")
    for city in cities:
        file_handle.write(str(city) + "\n")
    file_handle.close()


nearest_neighbor(sys.argv[-1])
