#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Apr 19 15:18:16 2020

@author: stevedeng
"""

from bokeh.io import output_file, show
from bokeh.models import ColumnDataSource, FactorRange
from bokeh.plotting import figure

initData = []
imprvData = []

file_header='graph_plotting_'
file_extension='.txt'
output_file("bar_graph_comparison_plot.html")

factors = [
    ("Random init", "BLP"), ("Random init", "BLP-MC"), ("Random init", "BLP-KL"),
    ("SBM init", "BLP"), ("SBM init", "BLP-MC"), ("SBM init", "BLP-KL"),
    ("Metis init", "BLP"), ("Metis init", "BLP-MC"), ("Metis init", "BLP-KL")

]

methods = ['initialization', 'improvement']

generatedResults = [ 
                         'Random_BLP_livejournal_50',
                         'Random_BLP-MC_livejournal_50',
                         'Random_BLP-KL_livejournal_50',
                         'SBM_BLP_livejournal_50',
                         'SBM_BLP-MC_livejournal_50',
                         'SBM_BLP-KL_livejournal_50',
                         'Metis_BLP_livejournal_50',
                         'Metis_BLP-MC_livejournal_50',
                         'Metis_BLP-KL_livejournal_50'
                ]

def readData():
    for method in generatedResults:
        fileName = file_header + method + file_extension
        print(fileName)
        inFile = open(fileName, 'r')
        title = inFile.readline()
        
        #after title line first line is init result
        init = inFile.readline().strip('\n').split(' ')[1]
        initData.append(float(init))
        
        #read remaining data and parse the last line to get final data
        final = inFile.readlines()[-1].strip('\n').split(' ')[1]
        improved = float(final) - float(init)
        imprvData.append(improved)
        
    print("Init Data: ", initData)
    print("Improvement Data: ", imprvData)

def main():
    
    readData()
    
    source = ColumnDataSource(data=dict(
        x=factors,
        initialization=initData,
        improvement=imprvData,
    ))
    
    p = figure(x_range=FactorRange(*factors), plot_height=650,
               toolbar_location=None, tools="")
    
    p.vbar_stack(methods, x='x', width=0.9, alpha=0.7, color=["blue", "red"], source=source,
                 legend_label=methods)
    
    p.y_range.start = 0
    p.y_range.end = 1
    p.x_range.range_padding = 0.3
    p.xaxis.major_label_orientation = 1
    p.xgrid.grid_line_color = None
    p.legend.location = "top_left"
    p.legend.orientation = "horizontal"
    p.xaxis.axis_label = "Initialization + Refinement Methods"
    p.yaxis.axis_label = "Locality Ratio"
    p.xaxis.axis_label_text_font_size = "15pt"
    p.yaxis.axis_label_text_font_size = "15pt"
    p.axis.axis_label_text_font_style = 'bold'
    p.xaxis.major_label_text_font_size = '5mm'
    p.yaxis.major_label_text_font_size = '5mm'
    
    show(p)
    
if __name__=='__main__':
    main()