#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Jul 22 13:41:59 2017

@author: stevedeng
"""
from bokeh.plotting import figure, show, output_file

output_file('graph_comparison_plot.html')
file_header='graph_plotting_'
file_extension='.txt'
#dataset layout dataset['methodName']=[ [Y(locality)] , [X(iterations)] ]
dataset={}

#for each line plot, x should be the iterations, y should be the locality

def readGeneratedFile(filename, methodname):
    inFile=open(filename, 'r')
    title=inFile.readline()
    itr=0
    
    #in generated file each file is a method
    iteration=[]
    locality=[]
    
    for line in inFile:
        line=line.strip('\n')
        line=line.split(' ')

        locality.append(line[1])
        iteration.append(itr)
        itr+=1
    
    dataset[methodname] = [locality, iteration]
    
def readManualFile(filename):
    inFile=open(filename, 'r')
    
    #in manual file each line is a method
    for line in inFile:
        line=line.strip('\n')
        line=line.split(', ')
        x_val = []
        y_val = []
        
        y_val.append(float(line[1]))
        y_val.append(float(line[1]))
        x_val.append(0)
        x_val.append(20)
        
        methodname=line[0]
        dataset[methodname]=[y_val, x_val]
    


def main():
    #gpmetis and comm + greedy are straight lines data that sit in the manual file
    generatedResults = ['cluster_move', 'pairwise_swap', 'Ugandar', 'disruptive']
    colors = {'cluster_move':'red', 'pairwise_swap':'blue', 'comm + greedy':'green', 'gpmetis':'orange', 'Ugandar':'grey', 'disruptive':'pink'}
    for method in generatedResults:
        filename = file_header + method + file_extension
        readGeneratedFile(filename, method)
    
    readManualFile('graph_plotting_comm_BLP.txt')
    
    plot1=figure(plot_width=400, plot_height=400, title="Locality against number of iterations")
    
    #dataset layout dataset['methodName']=[ [Y(locality)] , [X(iterations)] ]
    for key, value in dataset.items():
        methodName=key
        y_data=value[0]
        x_data=value[1]
        # add both a line and circles on the same plot
        plot1.line(x=x_data, y=y_data, line_width=2, color=colors[methodName], legend=methodName)
        plot1.circle(x=x_data, y=y_data, fill_color="white", size=8)
        
    plot1.legend.location = "bottom_right"
    
    
    show(plot1)

if __name__=='__main__':
    main()
