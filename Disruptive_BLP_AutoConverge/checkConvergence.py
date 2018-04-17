#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''  checkConvergence.py
//  Boost
//
//  Created by Steve DengZishi on 9/26/17.
//  Copyright © 2017 Steve DengZishi. All rights reserved.
//
//  We define convergence to be the case if the improvement in locality is less than 1% compare to the previous iteration
//
//  logic: read from the locality data after each round of BLP and check the improvement of locality whether less than 1%
//  if so, we say BLP converges and we will run a disruptive round from the bash script instead of run disruptive every
//  fixed number of rounds
'''
#read from recently added graph plotting data
th1=0.01
infile=open("graph_plotting_data.txt","r")
infile.readline()
node=[]
locality=[]

for line in infile:
    line=line.strip()
    line=line.split()
    node.append(line[0])
    locality.append(line[1])

#the difference in locality was the last locality minus off the second last
localityDiff=float(locality[-1])-float(locality[-2])
impRatio=localityDiff/float(locality[-2])
if impRatio<th1:
    print("TRUE")
    print(locality[-1])
else:
    print("FALSE")
    print(locality[-1])

infile.close()
