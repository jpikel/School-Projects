"""Filename: changeslowBF.py
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


#use helper function for total to be passed in recursively
def changeslowBF(V, amount):
    total = 1
    coinCounts, coinTotals = changeslowBF_helper(V, amount, total)
    return coinCounts, coinTotals
    """Function: changeslow
    Description: slow algorithm implementation for coinchange
    Parameters: Array of coin values an an amount of change to make
    Preconditions: Must have an array of integers initialized with at least one as 1
    Postconditions: returns array of coins used and minimum coins required
    """
def changeslowBF_helper(V, amount, total):

    # array to track usage of coins
    minCoins = [0] * len(V)

    # if coin in V is same as amount of change, add one to coin value in array, return array and total
    if amount in V:
        minCoins[V.index(amount)] += 1
        return minCoins, total

    minCoins[0] = amount

    # loop through value list to find minimum number of coins needed
    for i in [coin for coin in V if coin <= amount]:
        total1, temp = changeslowBF_helper(V, amount - i, total)
        total1[V.index(i)] += 1

        if sum(minCoins) > sum(total1):
            minCoins = total1
            total = sum(minCoins)

    return minCoins, total




FULL_FILE_NAME = sys.argv[-1] #argv[-1] is the filename i.e. Coin1.txt
coinchange.run_this(changeslowBF, "changeslow", FULL_FILE_NAME)
