#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Aug  5 12:14:07 2017

@author: stevedeng
"""

from bokeh.plotting import figure, show, output_file
from bokeh.layouts import gridplot

output_file('Comp_result_graph.html')

#x-axis is always number of partition
#y-axis can be locality or time
partition=[]
time_BLP=[]
time_METIS=[]
time_METIS_BLP_1=[]
time_METIS_BLP_2=[]
time_METIS_BLP_3=[]
locality_BLP=[]
locality_METIS=[]
locality_METIS_BLP_1=[]
locality_MEITS_BLP_2=[]
locality_METIS_BLP_3=[]

inFile=open('BLP_comp_data.txt','r')

title=inFile.readline()

for line in inFile:
    line=line.strip('\n')
    line=line.split(' ')
    partition.append(line[0])
    locality_BLP.append(line[1])
    locality_METIS.append(line[2])
    locality_METIS_BLP_1.append(line[3])
    locality_MEITS_BLP_2.append(line[4])
    locality_METIS_BLP_3.append(line[5])
    time_BLP.append(line[6])
    time_METIS.append(line[7])
    time_METIS_BLP_1.append(line[8])
    time_METIS_BLP_2.append(line[9])
    time_METIS_BLP_3.append(line[10])

plot1=figure(plot_width=600, plot_height=400, title="Locality comparison among BLP, METIS and METIS+BLP")
plot1.yaxis.axis_label = 'Fraction of edge locality %'
plot1.xaxis.axis_label = 'Number of partitions'
# add both a line and circles on the same plot
plot1.line(x=partition, y=locality_METIS, line_width=2, color="blue",legend="METIS")
plot1.line(x=partition, y=locality_BLP, line_width=2, color="green",legend="BLP_25")
plot1.line(x=partition, y=locality_METIS_BLP_1, line_width=2, color="red", legend="METIS+BLP_1")
plot1.circle(x=partition, y=locality_BLP, fill_color="white", size=5, legend="BLP_25")
plot1.circle(x=partition, y=locality_METIS, fill_color="white", size=5, legend="METIS")
plot1.circle(x=partition, y=locality_METIS_BLP_1, fill_color="white", size=5, legend="METIS+BLP_1")

plot2=figure(plot_width=600, plot_height=400, title="Time comparison among BLP, METIS and METIS+BLP")
plot2.yaxis.axis_label = 'Time to completion S'
plot2.xaxis.axis_label = 'Number of partitions'
# add both a line and circles on the same plot
plot2.line(x=partition, y=time_METIS, line_width=2, color="blue", legend="METIS")
plot2.line(x=partition, y=time_BLP, line_width=2, color="green", legend="BLP_25")
plot2.line(x=partition, y=time_METIS_BLP_1, line_width=2, color="red", legend="METIS+BLP_1")
plot2.circle(x=partition, y=time_BLP, fill_color="white", size=5, legend="BLP_25")
plot2.circle(x=partition, y=time_METIS, fill_color="white", size=5, legend="METIS")
plot2.circle(x=partition, y=time_METIS_BLP_1, fill_color="white", size=5, legend="METIS+BLP_1")
plot2.legend.location="center_right"

plot3=figure(plot_width=600, plot_height=400, title="Locality comparison among METIS + different iterations of BLP")
plot3.yaxis.axis_label = 'Fraction of edge locality %'
plot3.xaxis.axis_label = 'Number of partitions'
# add both a line and circles on the same plot
plot3.line(x=partition, y=locality_METIS_BLP_1, line_width=3, color="red", legend="METIS+BLP_1")
plot3.line(x=partition, y=locality_MEITS_BLP_2, line_width=2, color="navy", legend="METIS+BLP_2")
plot3.line(x=partition, y=locality_METIS_BLP_3, line_width=1, color="black", legend="METIS+BLP_3")
plot3.circle(x=partition, y=locality_METIS_BLP_1, fill_color="white", size=5, legend="METIS+BLP_1")
plot3.circle(x=partition, y=locality_MEITS_BLP_2, fill_color="white", size=5, legend="METIS+BLP_2")
plot3.circle(x=partition, y=locality_METIS_BLP_3, fill_color="white", size=5, legend="METIS+BLP_3")

plot4=figure(plot_width=600, plot_height=400, title="Time comparison among METIS + different iterations of BLP")
plot4.yaxis.axis_label = 'Time to completion S'
plot4.xaxis.axis_label = 'Number of partitions'
# add both a line and circles on the same plot
plot4.line(x=partition, y=time_METIS_BLP_1, line_width=2, color="red", legend="METIS+BLP_1")
plot4.line(x=partition, y=time_METIS_BLP_2, line_width=2, color="navy", legend="METIS+BLP_2")
plot4.line(x=partition, y=time_METIS_BLP_3, line_width=2, color="black", legend="METIS+BLP_3")
plot4.circle(x=partition, y=time_METIS_BLP_1, fill_color="white", size=5, legend="METIS+BLP_1")
plot4.circle(x=partition, y=time_METIS_BLP_2, fill_color="white", size=5, legend="METIS+BLP_2")
plot4.circle(x=partition, y=time_METIS_BLP_3, fill_color="white", size=5, legend="METIS+BLP_3")

p=gridplot(plot1,plot2,plot3,plot4,ncols=2)
show(p)


