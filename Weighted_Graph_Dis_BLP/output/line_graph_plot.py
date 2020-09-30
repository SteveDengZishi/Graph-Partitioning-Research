#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Jul 22 13:41:59 2017

@author: stevedeng
"""
from bokeh.plotting import figure, show, output_file

output_file('line_graph_comparison_plot.html')
file_header='graph_plotting_'
file_extension='.txt'
graphs_name = '_livejournal_50'
#dataset layout dataset['methodName']=[ [Y(locality)] , [X(iterations)] ]
dataset={}

#for each line plot, x should be the iterations, y should be the locality

def readGeneratedFile(filename, methodname):
    print(filename)
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
    
    #ALL PLOTS
    generatedResults = [ 
                         #'Random_BLP',
                         #'Random_BLP-MC',
                         #'Random_BLP-KL',
                         'SBM_BLP',
                         'SBM_BLP-MC',
                         'SBM_BLP-KL',
                         'Metis_BLP',
                         'Metis_BLP-MC',
                         'Metis_BLP-KL'
                ]
    
    colors = {
                'SBM':'green', 
                'Metis':'orange',
                'Random':'grey'
            }
    
    for method in generatedResults:
        filename = file_header + method + graphs_name + file_extension
        readGeneratedFile(filename, method)
    
    #gpmetis and comm + greedy are straight lines data that sit in the manual file
    #can be used for other manual graphs
    readManualFile('graph_plotting_manual.txt')
    
    plot1=figure(plot_width=600, plot_height=600, title="Locality against number of iterations")
    
    #dataset layout dataset['methodName']=[ [Y(locality)] , [X(iterations)] ]
    for key, value in dataset.items():
        methodName=key
        y_data=value[0]
        x_data=value[1]
        # add both a line and circles on the same plot
        # color is according to initialization technique and shape is according to refinement method
        init_method = methodName.split('_')[0]
        refine_method = methodName.split('_')[1]
        plot1.line(x=x_data, y=y_data, line_width=2, color=colors[init_method], legend_label=methodName.split('_')[1]+'('+ init_method +')')

        if refine_method == 'BLP':
            plot1.circle(x=x_data, y=y_data, fill_color="white", size=8, legend_label=methodName.split('_')[1]+'('+ init_method +')')
        elif refine_method == 'BLP-MC':
            plot1.triangle(x=x_data, y=y_data, fill_color="white", size=8, legend_label=methodName.split('_')[1]+'('+ init_method +')')
        elif refine_method == 'BLP-KL':
            plot1.asterisk(x=x_data, y=y_data, fill_color="white", size=8, legend_label=methodName.split('_')[1]+'('+ init_method +')')
        
    plot1.legend.location = "bottom_right"
    
    
    show(plot1)

if __name__=='__main__':
    main()
