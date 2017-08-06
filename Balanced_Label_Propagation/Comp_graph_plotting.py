#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Aug  5 12:14:07 2017

@author: stevedeng
"""

from bokeh.plotting import figure, show, output_file
from bokeh.layouts import column

output_file('Comp_result_graph.html')

iteration=[]
nodes=[]
locality=[]

inFile=open('BLP_comp_data.txt','r')

title=inFile.readline()
itr=0
for line in inFile:
    line=line.strip('\n')
    line=line.split(' ')
    nodes.append(line[0])
    locality.append(line[1])
    iteration.append(itr)
    itr+=1



plot1=figure(plot_width=400, plot_height=400, title="Number of node movement across iteration")

plot1.vbar(x=iteration, width=0.5, bottom=0, top=nodes, color="navy")

plot2=figure(plot_width=400, plot_height=400, title="Locality of edges across iteration")

# add both a line and circles on the same plot
plot2.line(x=iteration, y=locality, line_width=2, color="red")
plot2.circle(x=iteration, y=locality, fill_color="white", size=8)

p=column(plot1,plot2)
show(p)


