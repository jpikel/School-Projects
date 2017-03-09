"""Filename: genetic.py
Description: work in progress genetic algorithm
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
import random
import operator
import copy

#two constants, number of Generations and the size of each Generation
GENERATIONS = 500       #generations of populations to iterate
POPULATION = 200         #total population
MUTATE_PROB = 5         #as a percent of 100

#set default timer to use for timing algorithms
if sys.platform == "win32":
    DEFAULT_TIMER = time.clock
else:
    DEFAULT_TIMER = time.time

def read_in_file(FILE_NAME):
    """Function: read_in_file()
    Description: reads in the FILE_Name passed.  Constructs a list of lists
    in the following format [(int, int,int)]
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
                    #line.append(False)              #add a False for visited to this particular city
                    list_of_cities.append(line)
                except ValueError:
                    pass
       # print(list_of_cities)
        return list_of_cities
    except IOError:
        print("\nError Opening File.\n")
        return -1

def generate_population(list_1):
    """
    return a list_1 of list_1s of cities to generate a random population
    """
    population = []
    random.seed()

    for i in xrange(POPULATION):
        new_list = []
        new_list = copy.copy(list_1)
        random.shuffle(new_list)
        population.append(new_list)
    return population

def fitness(list_1, distance_matrix):
    """
    fitness is the total length of the tour of cities
    a lower fitness is better
    """
    fitness = 0
    for idx in range(0, len(list_1)-1):
        fitness += distance_matrix[list_1[idx]][list_1[idx+1]]

    fitness += distance_matrix[list_1[len(list_1)-1]][list_1[0]]
    #print(fitness)
    return fitness

def mutate(list_1):
    """
    this is the mutate function if the random integer is less than the MUTATE probability
    choose two nodes and swap them.
    """
    if random.randint(0, 100) <= MUTATE_PROB:
        pos_1 = random.randint(0, len(list_1)-1)
        pos_2 = random.randint(0, len(list_1)-1)
        temp = list_1[pos_1]
        list_1[pos_1] = list_1[pos_2]
        list_1[pos_2] =temp
    return list_1

def crossover(father, mother, distance_matrix):
    """
    pick either the first node from the mother or father to add to the child
    Then for each successive node from the mother or father is not part of the child
    and is the next least distance add that to the child.
    then if the child is still not the length it should be append any remaining left over
    nodes.
    """
    child_1 = []*len(father)
    #pick a random first node
    pick_first = random.randint(0, 1)
    if pick_first == 1:
        child_1.append(father[0])
    else:
        child_1.append(mother[len(mother)-1])

    idx = 1
    for _ in father:
        mother_dist = sys.maxsize
        father_dist = sys.maxsize
        mother_dist = distance_matrix[mother[idx-1]][mother[idx]]
        father_dist = distance_matrix[mother[idx-1]][mother[idx]]
        if mother_dist < father_dist:
            if mother[idx] not in child_1:
                child_1.append(mother[idx])
        else:
            if father[idx] not in child_1:
                child_1.append(father[idx])
        idx += 1
        if idx+1 >= len(father)-1:
            idx = 0
    while len(child_1) < len(father):
        if mother[idx] not in child_1:
                child_1.append(mother[idx])
        if father[idx] not in child_1:
                child_1.append(father[idx])
        idx += 1
        if idx > len(father):
            idx = 0
        #print(child_1)
    return child_1

def crossover_2(father, mother):
    """
    2 point crossover and fill in the rest with first avaialble
    """
    child_1 = []
    random.seed()
    pos_1 = random.randint(0, len(father)/2)
    pos_2 = random.randint(pos_1, len(father)-1)
    child_1 = father[pos_1:pos_2]
    for idx in range(0, pos_1):
        if mother[idx] not in child_1:
            child_1.insert(idx, mother[idx])
        elif father[idx] not in child_1:
            child_1.insert(idx,father[idx])
    for idx in range(pos_2, len(father)-1):
        if mother[idx] not in child_1:
            child_1.append(mother[idx])
        elif father[idx] not in child_1:
            child_1.append(father[idx])
    idx = 0
    while len(child_1) < len(father):
        if mother[idx] not in child_1:
                child_1.append(mother[idx])
        if father[idx] not in child_1:
                child_1.append(father[idx])
        idx += 1
        if idx > len(father):
            idx = 0

    return child_1

def crossover_3(father, mother):
    """two point crossover between mother and father"""
    child = [None]*len(father)
    start_pos = int(math.floor(random.randint(0, len(father)/2)))
    end_pos = int(math.floor(random.randint((len(father)/2)+1, len(father)-1)))
    for idx in range(0, len(child)):
        if idx < start_pos or idx > end_pos:
            child[idx] = father[idx]
    child_idx = start_pos
    for idx in range(0, len(mother)):
        if mother[idx] not in child:
            child[child_idx] = mother[idx]
            child_idx += 1
    return child


def weighted_choice(population):
    """
    tournamet style parental selection
    pick only a certain individual from a subset of the population
    a certain number of times and find the best of that subset.
    """
    parent_1_weight = sys.maxsize
    parent_2_weight = sys.maxsize
    parent_1 = None
    parent_2 = None
    random.seed()
    probability = random.randint(1,len(population)*0.10)
    pop_range = random.randint(0, len(population)-1)
    for _ in range(0, probability):
        individual = population[random.randint(0, pop_range)]
        if individual[1] < parent_1_weight:
            parent_1_weight = individual[1]
            parent_1 = individual[0]
        if individual[1] > parent_1_weight and individual[1] < parent_2_weight:
            parent_2_weight = individual[1]
            parent_2 = individual[0]
    if parent_1 == None:
        parent_1 = population[random.randint(0, len(population)-1)][0]
    if parent_2 == None:
        parent_2 = population[random.randint(0, len(population)-1)][0]

    #incest prevention if the two parents are identical pick a different parent_2
    while parent_1 == parent_2:
        parent_2 = population[random.randint(0, len(population)-1)][0]
    return parent_1, parent_2

def sorted_choice(population):
    """sorts the population by the weight then picks the best individual from the list
    and then choose a random individual as that individual's mate
    """
    sorted_population = sorted(population, key=lambda x:x[1])
    random.seed()
    parent_1 = sorted_population[0][0]
    parent_2 = sorted_population[random.randint(1, len(population)-1)][0]
    while parent_1 == parent_2:
        parent_2 = sorted_population[random.randint(1, len(population)-1)][0]
    return parent_1, parent_2



def genetic(FILE_NAME):
    """
    this will hopefully be a genetic algorithm implementation of the TSP.
    I used a lot of references while reaserching this algorithm and one of my main
    sources is https://gist.github.com/cfdrake/973505
    https://iccl.inf.tu-dresden.de/w/images/b/b7/GA_for_TSP.pdf
    """
    time_0 = DEFAULT_TIMER()
    list_of_cities = read_in_file(FILE_NAME)
    cities_only = []
    sol_of_cities = []                  #stores current solution being worked on
    opt_distance = 0                    #store current optimal distance
    best_sol_of_cities = []             #best overall solution
    best_opt_distance = sys.maxsize     #best optimal distance
    random.seed()                   #seed random generator

    #compute the distance matrix for all cities to all other cities
    distance_matrix = [[] for _ in range(0, len(list_of_cities))]

    #we only want a list of the city numbers
    for city in list_of_cities:
        cities_only.append(city[0])

    population = generate_population(cities_only)

    #create the distance matrix for all cities to all other cities
    for city_1 in list_of_cities:
        for city_2 in list_of_cities:
            dx = city_1[1] - city_2[1]
            dy = city_1[2] - city_2[2]
            distance_matrix[city_1[0]].append(int(round(math.sqrt((dx*dx) + (dy*dy)))))

    for generation in xrange(GENERATIONS):
        #print(generation)
        weighted_population = []
        for individual in population:
            fitness_val = fitness(individual, distance_matrix) 
            #fitness_val is the length of the tour

            weighted_population.append((individual, fitness_val))

        population = []
        for _ in xrange(POPULATION/2):
            father, mother = weighted_choice(weighted_population)
            #child_1 = crossover(father, mother, distance_matrix)
            child_1 = crossover_3(father, mother)
            father, mother = weighted_choice(weighted_population)
            child_2 = crossover_3(father, mother)
            population.append(mutate(child_1))
            population.append(mutate(child_2))
            #father, mother = sorted_choice(weighted_population)
            #child_2 = crossover(father, mother, distance_matrix)
            #population.append(mutate(child_2))

    best_opt_distance = sys.maxsize
    for individual in population:
        fitness_val = fitness(individual, distance_matrix)
        if fitness_val < best_opt_distance:
            best_opt_distance = fitness_val
            best_sol_of_cities = individual

    print(best_opt_distance)
    write_to_file(best_sol_of_cities, best_opt_distance, FILE_NAME)
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


genetic(sys.argv[-1])
