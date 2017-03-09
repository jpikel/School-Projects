"""Filename: changegreedy.py
    Authors: Kelsey Helms, Jay Steingold, Johannes Pikel
    Date: 2016.10.18
    Class: CS325-400
    Assignment: Project 2
    Python version: script written for Python version 2.7.5
"""

#from __future__ import print_function
import sys
#import time
#import os
#import random
#import math
import coinchange

#check Python version
if sys.version_info < (2, 7):
    input("Your Python interpreter may be too old, please upgrade to at least 2.7.5")
    sys.exit()

def changegreedy(V, A):
    """Function: changegreedy
    Description: greedy algorithm implementation for coinchange
    Parameters: Array of coin values an an amount of change to make
    Preconditions: Must have an array of integers initialized with at least one as 1
    Postconditions: returns array of coins used and minimum coins required
    """
    largest = len(V) - 1
    totalCoins = 0
    C = [0] * (len(V))
    while largest >= 0:
        if V[largest] == A:
            C[largest] = C[largest] + 1
            totalCoins += 1
            return (C, totalCoins)
        if V[largest] > A:
            largest = largest - 1
        if V[largest] < A:
            A = A - V[largest]
            totalCoins = totalCoins + 1
            C[largest] = C[largest] + 1
    return (C, totalCoins)

FULL_FILE_NAME = sys.argv[-1] #argv[-1] is the filename i.e. Coin1.txt
coinchange.run_this(changegreedy, "changegreedy", FULL_FILE_NAME)
