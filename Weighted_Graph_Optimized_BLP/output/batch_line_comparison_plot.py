#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Apr 26 15:52:41 2020

@author: stevedeng
"""

from bokeh.plotting import figure, show, output_file

file_header='graph_plotting_'
file_extension='.txt'

graphs = [ 'livejournal.txt', 'roadNetCA.txt', 'email-Enron.txt', 'orkut.txt' ]

#ALL PLOTS
generatedResults = [ 
                     'Random_BLP',
                     'Random_BLP-MC',
                     'Random_BLP-KL',
                     'SBM_BLP',
                     'SBM_BLP-MC',
                     'SBM_BLP-KL',
                     'Metis_BLP',
                     'Metis_BLP-MC',
                     'Metis_BLP-KL'
                 ]

#use the first data point which is the initialization but not the last data point as in generated results
benchmarks = [
                'SBM_BLP',
                'Metis_BLP'
            ]

colors = {
            'SBM':'royalblue', 
            'Metis':'red',
            'Random':'chocolate'
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
        print(method.split('_')[0],file=raw_data_file, end=' ')
        result_lst = []
        for part in partitions:
            filename = folder_name + '/' + file_header + method + '_' + folder_name + '_' + str(part) + file_extension
            final_locality = readBenchmarkFile(filename, raw_data_file)
            result_lst.append(final_locality)
            
        print('', file=raw_data_file) 
        result_map[method.split('_')[0]] = [result_lst, partitions]
        
    
    raw_data_file.close()
    plot1=figure(plot_width=700, plot_height=700, title= folder_name + " Graph - Final Locality (Y) against number of partitions (X)")
    
    #ploting each method lines    
    for key, value in result_map.items():
        methodName=key
        y_data=value[0]
        x_data=value[1]

        init_method = methodName.split('_')[0]
        refine_method = ""

        if len(methodName.split('_')) > 1:
            refine_method = methodName.split('_')[1]
        
        # add both a line and another pattern representing refinement methods
        plot1.line(x=x_data, y=y_data, line_width=2, color=colors[init_method], legend_label=refine_method+'('+ init_method +')')

        if refine_method == 'BLP':
            plot1.square(x=x_data, y=y_data, fill_color="lavenderblush", size=8, legend_label=refine_method+'('+ init_method +')')
        elif refine_method == 'BLP-MC':
            plot1.circle(x=x_data, y=y_data, fill_color="lavenderblush", size=8, legend_label=refine_method+'('+ init_method +')')
        elif refine_method == 'BLP-KL':
            plot1.triangle(x=x_data, y=y_data, fill_color="lavenderblush", size=8, legend_label=refine_method+'('+ init_method +')')
    
    #plot stylings  
    plot1.legend.location = "top_right"
    plot1.xaxis.axis_label = "Number of partitions"
    plot1.yaxis.axis_label = "Locality Ratio"
    plot1.title.text_font_size = "13pt"
    plot1.xaxis.axis_label_text_font_size = "15pt"
    plot1.yaxis.axis_label_text_font_size = "15pt"
    plot1.axis.axis_label_text_font_style = 'bold'
    plot1.xaxis.major_label_text_font_size = '5mm'
    plot1.yaxis.major_label_text_font_size = '5mm'
    
    show(plot1)
        


def main():
    for graph in graphs:
        generatePlotForGraph(graph)
    

if __name__=='__main__':
    main()
