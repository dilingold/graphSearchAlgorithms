#! /usr/bin/env python
"""
A simple Python script for generating data for the assignment.
- David Shaw 2014
"""

import random
import sys

# the choice of what is in the squares is randomly chosen from this list.
choice_list = ['.',':','=','+'];

def rowlist():

	return [random.choice(choice_list) for x in range(16)]
rowgrid = [rowlist() for x in range(6)]


for y in range(30):
	for x in range(80):
		c = '#';
		xi = (x)/5
		yi = (y)/5
		c = rowgrid[yi][xi]

		sys.stdout.write(c)
	sys.stdout.write('\n')

sys.stdout.flush()