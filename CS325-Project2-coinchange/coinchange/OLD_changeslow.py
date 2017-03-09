"""Filename: changeslow.py
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


def changeslow(denominations, change_to_make):
    """Function: changeslow
    Description: divide and conquer algorithm implementation for coinchange
    Parameters: array of denominations, int of change to make
    Preconditions: valid denominations, positive int
    Postconditions: returns array of counts for denominations used and minimum number of coins required
    as int
    Cite: Referenced https://www.cis.upenn.edu/~matuszek/cit594-2014/Lectures/30-dynamic-programming.ppt
    """
    num_coins = len(denominations)

    for i in range(0, num_coins):
        if(denominations[i] == change_to_make):
            solution = [0] * num_coins
            solution[i] = 1
            return solution, 1

    solution = [0] * num_coins
    total_coins = sys.maxint
    for i in range(1, change_to_make):
        solution1, count1 = changeslow(denominations, i)
        solution2, count2 = changeslow(denominations, change_to_make - i)

        new_coin_count = count1 + count2
        if new_coin_count < total_coins:
            total_coins = new_coin_count
            for i in range(0, len(solution)):
                solution[i] = solution1[i] + solution2[i]

    return solution, total_coins




FULL_FILE_NAME = sys.argv[-1] #argv[-1] is the filename i.e. Coin1.txt
coinchange.run_this(changeslow, "changeslow", FULL_FILE_NAME)
