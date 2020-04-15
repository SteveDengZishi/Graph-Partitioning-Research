#!/bin/bash

#  executePairwise_BLP.sh
#  Hybrid pairwise partition swap plus linear programming (HCLP)
#  With pairwise partitions swapping at convergence
#  Boost
#
#  Created by Steve DengZishi on 11/28/19.
#  Copyright © 2019 Steve DengZishi. All rights reserved.
echo -e "\nComm + Greedy with Ugandar Balanced Label Propagation  Version 3.0"
echo -e "       Copyright © 2019 Steve DengZishi  New York University\n"
#prompt user for input
read -p "Please enter the fileName: " FileName
read -p "Please enter the number of partition k (k<1000): " shard
read -p "Enter the number of iterations of Ugandar move: " iter
read -p "Enter the seed of random probability: " seed
read -p "Enter 'v' to verbose clustering movement outputs or press enter to skip: " verbose
#compile all .cpp files to executables
g++ -o clus clus.cpp -std=c++11
echo -e "\ng++ compiled clus.cpp successfully"
g++ -o greedyAssignment greedyAssignment.cpp -std=c++11
echo -e "g++ compiled greedyAssignment.cpp successfully"
g++ -o clean clean.cpp -std=c++11
echo -e "g++ compiled clean.cpp successfully"
g++ -o lp_ingredient_producer_individual lp_ingredient_producer_individual.cpp -std=c++11
echo -e "g++ compiled lp_ingredient_producer_individual.cpp successfully"
g++ -o linear linear.cpp -std=c++11
echo -e "g++ compiled linear.cpp successfully"
g++ -o applyMove applyMove.cpp -std=c++11

IFS="." read -ra file <<< "$FileName"
clusterFile="${file[0]}_clusters.txt"
echo -e "Searching for clustered community file: $clusterFile\n"

if [ ! -f "$clusterFile" ]
then
echo -e "Running Clustering Algorithm...\n"
./clus $FileName $seed $verbose
else
echo -e "Skipping Clustering as file exists...\n"
fi

#Random initialization with clusters
echo -e "Starting greedy initialization...\n"
./greedyAssignment $FileName $shard $verbose

echo -e "Initialization completed\n"

#start of iteration
for((i=1;i<iter+1;i++))
do
echo -e "\nIn iteration" $i

echo -e "Running individual moving round\n"
./lp_ingredient_producer_individual $FileName $shard > lp_ingred.txt

./linear < lp_ingred.txt > lp_source.txt
lp_solve < lp_source.txt > lp_rawResult.txt
./clean < lp_rawResult.txt | sort > x_result_$i.txt

x_file=x_result_$i.txt

./applyMove $FileName $shard $x_file

done

#plotting graph after finish looping
chmod +x graph_plot.py
./graph_plot.py
