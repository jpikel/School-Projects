"""Filename: changedp.py
    Authors: Kelsey Helms, Jay Steingold, Johannes Pikel
    Date: 2016.10.18
    Class: CS325-400
    Assignment: Project 2
    Python version: script written for Python version 2.7.5
"""

from __future__ import print_function
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

def changedp(denominations, change_to_make): #denominations is a list of ints, change_to_make is int
    """Function: changedp
    Description: dynamic programming algorithm implementation for coinchange
    iterates from 1 cent to kth cents where k = change to make.  For each i cent
    compares to denominations to see if the denomination is smaller than i and if
    plus 1 coin would be less than the coins currently being used for that denomination.
    Stores the coins used in a set and also the denominations used.
    then to retrieve the denominations used, starts from the end of the coins_used set 
    collecting the denominations used and adds 1 to the count in the relative index location
    of that denomination in the result array.
    Parameters: array of ints, int
    Preconditions: passed valid array of denominations and positive integer for change_to_make
    Postconditions:returns array of counts of denominations used
    and integer for minimum number of coins required to make change.
    Cite:http://ace.cs.ohiou.edu/~razvan/courses/cs4040/lecture19.pdf
    """
    #print("denoms: ", denominations)

    total_coins = {}
    coins_used = {}
    total_coins[0] = 0
    for i in range(1, change_to_make + 1):#change to be made
        total_coins[i] = sys.maxsize
        for j in range(0, len(denominations)):#number of denominations
            if i >= denominations[j] and 1 + total_coins[i - denominations[j]] < total_coins[i]:
                total_coins[i] = 1 + total_coins[i - denominations[j]]
                coins_used[i] = denominations[j]

    denom_dict = {} #dictionary storing key = denomination: value = index location
    result = [0] * len(denominations) #store the count of each coin used
    index = 0
    for denom in denominations: #create a dictionary of the denominations and their index
        denom_dict[denom] = index
        index += 1
    key = change_to_make
    while key > 0:
        value = coins_used.get(key)
        result[denom_dict.get(value)] += 1  #add 1 coin to the denominations index location
        key = key - coins_used[key]         #move backwards through coins_used by index - value stored.

    return result, total_coins[change_to_make]  #[1, 2] is a default return value for now to test writing to file

FULL_FILE_NAME = sys.argv[-1] #argv[-1] is the filename i.e. Coin1.txt
coinchange.run_this(changedp, "changedp", FULL_FILE_NAME)
