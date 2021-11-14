#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Aug 16 16:20:19 2020

@author: stevedeng
"""

from bokeh.plotting import figure, show, output_file

output_file('line_graph_comparison_plot.html')

#ALL PLOTS
generatedResults = [ 
                     'random_Ugander',
                     'commGreedy'
                 ]

colors = {
            'Community Sharding':'navy', 
            'Constrained Relocation':'red'
        }
#dataset layout dataset['methodName']=[ [Y(locality)] , [X(partitions)] ]
dataset={}

#for each line plot, x should be the iterations, y should be the locality
    
def readManualFile(filename):
    inFile=open(filename, 'r')
    
    x_Ugander_val = []
    y_Ugander_val = []
    x_Duong_val=[]
    y_Duong_val=[]
    #in manual file each line is a method
    for line in inFile:
        line=line.strip('\n')
        line=line.split(',')
        
        x_Ugander_val.append(int(line[0]))
        x_Duong_val.append(int(line[0]))
        y_Ugander_val.append(float(line[1]))
        y_Duong_val.append(float(line[2]))
        
        dataset['Constrained Relocation']=[y_Ugander_val, x_Ugander_val]
        dataset['Community Sharding']=[y_Duong_val, x_Duong_val]

def main():
    
    filename = "Ugander_Duong.txt"
    
    readManualFile(filename)
    
    plot1=figure(plot_width=600, plot_height=600, title="Final locality against number of partitions", x_axis_label="Number of partitions", y_axis_label="Final local edge ratio (Locality)")
    
    #dataset layout dataset['methodName']=[ [Y(locality)] , [X(iterations)] ]
    for key, value in dataset.items():
        methodName=key
        y_data=value[0]
        x_data=value[1]
        # add both a line and circles on the same plot
        plot1.line(x=x_data, y=y_data, line_width=2, color=colors[methodName], legend=methodName)
        plot1.circle(x=x_data, y=y_data, fill_color="white", size=8)
        
    plot1.legend.location = "top_right"
    
    
    show(plot1)

if __name__=='__main__':
    main()
