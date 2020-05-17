#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Apr 26 15:52:41 2020

@author: stevedeng
"""

from bokeh.plotting import figure, show, output_file

file_header='graph_plotting_'
file_extension='.txt'

graphs = [ 'livejournal.txt', 'roadNetCA.txt', 'email-Enron.txt', 'athletes_edges_clean.txt', 'company_edges_clean.txt', 'orkut.txt' ]

#ALL PLOTS
generatedResults = [ 
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

benchmarks = [
                'commGreedy_Ugandar',
                'gpmetis_Ugandar'
            ]

colors = {
            'commGreedy_clusterMove':'red', 
            'commGreedy_pairwiseSwap':'blue',
            'commGreedy_Ugandar':'black',
            'commGreedy':'brown', 
            'gpmetis_clusterMove':'gold',
            'gpmetis_Ugandar':'purple', 
            'gpmetis_pairwiseSwap':'green',
            'gpmetis':'orange',
            'random_Ugandar':'grey', 
            'random_pairwiseSwap':'pink',
            'random_clusterMove':'silver'
        }

partitions = [ 10, 30, 50, 70, 90 ]

#for each line plot, x should be the iterations, y should be the locality

def readGeneratedFile(filename, raw_data_file):
    print(filename)
    inFile=open(filename, 'r')
    lines=inFile.readlines()
    print(lines[-1].strip('\n').split(' ')[-1], file=raw_data_file, end=' ')
    #return last field on the last line final locality
    return lines[-1].strip('\n').split(' ')[-1]

def readBenchmarkFile(filename, raw_data_file):
    print(filename)
    inFile=open(filename, 'r')
    lines=inFile.readlines()
    print(lines[1].strip('\n').split(' ')[-1], file=raw_data_file, end=' ')
    #return last field on the first line final locality representing benchmark locality
    return lines[1].strip('\n').split(' ')[-1]
    
    
def generatePlotForGraph(graph_name):
    
    folder_name = graph_name.split('.')[0];
    output_file('line_graph_comparison_' + folder_name + '_plot.html')
    
    #result_map layout result_map['methodName']=[ [Y(locality)] , [X(iterations)] ]
    result_map = {}
    
    raw_file = folder_name + '_raw.txt'
    raw_data_file = open(raw_file, 'w')
    
    for method in generatedResults:
        print(method,file=raw_data_file, end=' ')
        result_lst = []
        for part in partitions:
            filename = folder_name + '/' + file_header + method + '_' + folder_name + '_' + str(part) + file_extension
            final_locality = readGeneratedFile(filename, raw_data_file)
            result_lst.append(final_locality)
            
        print('', file=raw_data_file) 
        result_map[method] = [result_lst, partitions]
    
    #benchmarks file is for recording initialization methods performances alone, has a different parse logic to parse the initial locality before refinements
    for method in benchmarks:
        print(method,file=raw_data_file, end=' ')
        result_lst = []
        for part in partitions:
            filename = folder_name + '/' + file_header + method + '_' + folder_name + '_' + str(part) + file_extension
            final_locality = readBenchmarkFile(filename, raw_data_file)
            result_lst.append(final_locality)
            
        print('', file=raw_data_file) 
        result_map[method.split('_')[0]] = [result_lst, partitions]
        
    
    raw_data_file.close()
    plot1=figure(plot_width=700, plot_height=600, title= folder_name + " Graph - Final Locality (Y) against number of partitions (X)")
    
    #ploting each method lines    
    for key, value in result_map.items():
        methodName=key
        y_data=value[0]
        x_data=value[1]
        
        # add both a line and circles on the same plot
        plot1.line(x=x_data, y=y_data, line_width=2, color=colors[methodName], legend=methodName)
        plot1.circle(x=x_data, y=y_data, fill_color="white", size=8)
        
    plot1.legend.location = "top_right"
    
    show(plot1)
        


def main():
    for graph in graphs:
        generatePlotForGraph(graph)
    

if __name__=='__main__':
    main()
