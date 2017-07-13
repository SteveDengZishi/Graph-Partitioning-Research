#!/bin/bash

#  executeBLPG_script.sh
#  Boost
#
#  Created by Steve DengZishi on 7/12/17.
#  Copyright © 2017 Steve DengZishi. All rights reserved.

#set the source file for input
FileName=facebook_combined.txt
#echo $FileName

#execute ReadData with input graph
for num in `seq 1 10`
do
./ReadData < $FileName
done
