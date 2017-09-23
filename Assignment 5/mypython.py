#!/bin/env python

import sys 
import random
from random import randint

def makefiles() :	
	f1 = open("file1.txt","w");
	for i in range(0,10):
		rand = randint(97,122)
		f1.write(chr(rand))
	f1.close()
        f2 = open("file2.txt","w");
        for h in range(0,10):
                rand = randint(97,122)
                f2.write(chr(rand))
        f2.close()
        f3 = open("file3.txt","w");
        for j in range(0,10):
                rand = randint(97,122)
                f3.write(chr(rand))
        f3.close()
	return 0

def readfiles() :
	with open("file1.txt") as fn:
		content = fn.readlines()
 	print "File 1 content: ", content
        with open("file2.txt") as fn:
                content = fn.readlines()
	print "File 2 content: ", content
        with open("file3.txt") as fn:
                content = fn.readlines()
        print "File 3 content: ", content
	return 0

def main () :
	makefiles()
	readfiles()
	rand1 = randint(1,43)
	rand2 = randint(1,43)
	product = rand1 * rand2
	print "Random integer one: ", rand1
	print "Random integer two: ", rand2
	print "Product: ", product
	return 0;

main()



