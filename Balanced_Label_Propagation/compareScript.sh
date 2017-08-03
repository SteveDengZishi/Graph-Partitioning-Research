#!/bin/bash
#  executeBLPG_script.sh
#  Boost
#
#  Created by Steve DengZishi on 7/12/17.
#  Copyright © 2017 Steve DengZishi. All rights reserved.
echo -e "\nBalanced Label Propagation Lateral Comparison Runner               Version 1.1"
echo -e "           Copyright © 2017 Steve DengZishi  New York University\n"

#set the source file for input
read -p "Enter the file name of the undirected graph: " FileName
read -p "Enter the maximum number of partitions k, it will run from 2 to k (k<1000): " shard
#prompt user input
read -p "Enter the number of iteration to carry out Balanced Label Propagation: " iter

for((i=2;i<=shard;i++))
do
./executeBLP_script.sh<<EOF
$FileName
$i
$iter
EOF
done
