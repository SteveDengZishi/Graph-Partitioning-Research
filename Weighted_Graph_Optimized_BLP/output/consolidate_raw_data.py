#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jan 31 16:40:00 2020

@author: stevedeng
"""
#this script is only used for consolidating all data into a readable .txt file not related to graph plotting

file_header='graph_plotting_'
file_extension='.txt'

def consolidateRawData(filename, methodname):
    inFile=open(filename, 'r')
    #skip title
    inFile.readline()
    
    #in generated file each file is a method
    locality=[]
    
    for line in inFile:
        line=line.strip('\n')
        line=line.split(' ')

        locality.append(line[1])
        
    outFile=open('raw_data.txt', 'a')
    print(methodname, file=outFile)
    for val in locality:
        print(val, end=' ', file=outFile)
    print('\n', file=outFile)
    
    inFile.close()
    outFile.close()
    
def main():
    #reset output file
    outFile=open('raw_data.txt', 'w')
    outFile.close()
    
    methodList = [ 
                         'random_Ugandar',
                         'random_clusterMove',
                         'random_pairwiseSwap',
                         'commGreedy_Ugandar',
                         'commGreedy_clusterMove',
                         'commGreedy_pairwiseSwap',
                         'gpmetis_Ugandar',
                         'gpmetis_clusterMove',
                         'gpmetis_pairwiseSwap'
                     ]
    
    for method in methodList:
        filename = file_header + method + file_extension
        consolidateRawData(filename, method)
        
if __name__=='__main__':
    main()