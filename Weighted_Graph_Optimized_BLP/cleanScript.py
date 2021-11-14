#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Oct 26 13:22:47 2019

@author: stevedeng
"""

#graph dataset clean script to format csv facebook_data
#convert all csv files under the current directory to desired format below
#node_count edge_count
#0 1223
#0 120
#....

import pandas as pd
import glob
import os

os.chdir('facebook_clean_data/')
extension='csv'
all_csv_files = glob.glob('*.{}'.format(extension))

#filename = input('Please input the file name: ')
for filename in all_csv_files:
    print('Processing file: ', filename)
    df = pd.read_csv(filename)
    
    node_count = max(max(df['node_1']),max(df['node_2']))+1
    edge_count = len(df)
    
    output_file = open(filename.split('.')[0]+'_clean.txt', 'w')
    output_file.write(str(node_count) + ' ' + str(edge_count) + '\n')
    
    for i in range(edge_count):
        print(df['node_1'][i], df['node_2'][i], file=output_file)
        
    output_file.close()
