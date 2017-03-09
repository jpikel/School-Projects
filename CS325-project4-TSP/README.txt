# project4
CS325-400-project4-group6
Members: Kelsey Helms, Jay Steingold, Johannes Pikel

All our algorithms are written in Python.

Of all the algorithms we implemented we got the best results from the nearest_dblimp2.py for the tsp_example_1.txt 
and tsp_example_3.txt.  For tsp_example_2.txt a repetitive nearest neighbor search produced our best results, called
nearest_neighbor.py

For the tsp_example instances please run nearest_dblimp2.py and nearest_neighbor.py

In order to run any of our algorithms you may do so with python <algorithm> <textfile>
So for instance python nearest_dblimp2.py tsp_example_1.txt would run nearest_dblimp2.py computing the best approximate tour for
tsp_example_1.txt

All our algorithms then write to a file with .tour appended to the file name. In the example above the output file would be
tsp_example_1.txt.tour

A brief description of the algorithms:
nearest_insertion.py (implementation of the nearest insertion algorithm)

greedy_insertion.py  (implementation of the greedy insertion algorithm)

**nearest_neighbor.py (a repetitive nearest neighbor search with a short circuit, if a current tour is larger than previous optimal)

**nearest_dblimp2.py (nearest neighbor search, with a weighted distance matrix for distant cities, double ended search, starting
                    from the two cities that are closest to each other)
                    
nearest_dblimp.py  (the same search as above, but not yet totally optimized on the process, has extra FOR loops that could be
                    combined)
                    
nearest_fast.py    (performs a double ended search beginning starting with the first and last city in the list)

nearest_imp.py     (performs a double ended search from the two cities closest to each other)

genetic.py         (a somewhat workable genetic algorithm implementation, requires tweaking of the POPULATION, GENERATIONS, and
                    MUTATE_PROB)

               
                    


