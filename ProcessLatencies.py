#!/usr/bin/env python

###################################################################################################
# Project: Latency
# Description: Module used to measure latencies in threads.
# Author: vmoreno
# Date: 04/03/2019
# Language: Python
# Version: 1.0
# History:
#		   04/03/2019 - vmoreno - File creation
###################################################################################################

import csv


def main():
    # Get data for 1 thread (main -> 20 ms)
    lat_thread1 = [(Array[0][index] - Array[0][index - 1])*1000000000 for index in range(1, len(Array[0]))]
    abovePeriod = 0
    totalElements = 0
    period = 20500000		# Period in ns
    elements_above_period = []
    for value in lat_thread1:
        totalElements += 1
        if value > period:
            elements_above_period.append(value)
            abovePeriod += 1
    print("THREAD 20 MS")
    print("Elements above period: ", abovePeriod)
    if abovePeriod > 0:
        print("% elements above period: ", (abovePeriod/totalElements) * 100)
        print("Average of elements above period (ns): ", int(sum(elements_above_period) / float(len(elements_above_period))))
    print("-----------------------------------------------------------")

    # Get data from second thread (16 ms)
    lat_thread2 = [(Array[1][index] - Array[1][index - 1])*1000000000 for index in range(1, len(Array[1]))]
    abovePeriod = 0
    totalElements = 0
    period = 16500000
    elements_above_period = []
    for value in lat_thread2:
        totalElements += 1
        if value > period:
            elements_above_period.append(value)
            abovePeriod += 1
    print("THREAD 16 MS")
    print("Elements above period: ", abovePeriod)
    if abovePeriod > 0:
        print("% elements above period: ", (abovePeriod/totalElements) * 100)
        print("Average of elements above period (ns): ", int(sum(elements_above_period) / float(len(elements_above_period))))
    print("-----------------------------------------------------------")

    # Get data from third thread (50 ms)
    lat_thread3 = [(Array[2][index] - Array[2][index - 1])*1000000000 for index in range(1, len(Array[2]))]
    abovePeriod = 0
    totalElements = 0
    period = 50500000
    elements_above_period = []
    for value in lat_thread3:
        totalElements += 1
        if value > period:
            elements_above_period.append(value)
            abovePeriod += 1
    print("THREAD 50 MS")
    print("Elements above period: ", abovePeriod)
    if abovePeriod > 0:
        print("% elements above period: ", (abovePeriod/totalElements) * 100)
        print("Average of elements above period (ns): ", int(sum(elements_above_period) / float(len(elements_above_period))))
    print("-----------------------------------------------------------")


if __name__ == '__main__':
    # Receive from command line: Latency file, number of threads and their periods.

    # Array to store times of latencies, grouped by threads.
    Array = [[], [], []]
    # Open file.
    with open('Latency.csv', newline='') as f:
        reader = csv.reader(f, delimiter=',')
        for row in reader:
            Array[int(row[0]) - 1].append(float(row[1]))


    # Call main
    main()