#!/bin/bash

#  Cluster_HCLP.sh
#  Hybrid clustered plus linear programming (HCLP)
#  Boost
#
#  Created by Steve DengZishi on 1/7/19.
#  Copyright © 2019 Steve DengZishi. All rights reserved.
echo -e "\nClustered Prob-Disruptive Balanced Label Propagation  Version 1.6"
echo -e "       Copyright © 2019 Steve DengZishi  New York University\n"
#prompt user for input
read -p "Please enter the fileName: " FileName
read -p "Please enter the number of clusters: " cluster
read -p "Please enter the number of partition: " shard

#compile all .cpp files to executables
g++ -o clus clus.cpp -std=c++11
echo -e "\ng++ compiled clus.cpp successfully"
g++ -o greedyAssignment greedyAssignment.cpp -std=c++11
echo -e "g++ compiled RandomAssignment.cpp successfully"
g++ -o clean clean.cpp -std=c++11
echo -e "g++ compiled clean.cpp successfully"
g++ -o lp_ingredient_producer_clus lp_ingredient_producer_clus.cpp -std=c++11
echo -e "g++ compiled lp_ingredient_producer_clus.cpp successfully"
g++ -o lp_ingredient_producer_individual lp_ingredient_producer_individual.cpp -std=c++11
echo -e "g++ compiled lp_ingredient_producer_individual.cpp successfully"
g++ -o linear linear.cpp -std=c++11
echo -e "g++ compiled linear.cpp successfully"
g++ -o applyMove applyMove.cpp -std=c++11
echo -e "g++ compiled applyMove.cpp successfully"
g++ -o probDisruptiveMove probDisruptiveMove.cpp -std=c++11
echo -e "g++ compiled disruptiveMove.cpp successfully\n"
chmod +x checkConvergence.py

echo -e "Running Clustering Algorithm...\n"
./clus $FileName $cluster

#Random initialization with clusters
echo -e "Starting greedy initialization...\n"
./greedyAssignment $FileName $shard

echo -e "Initialization completed\n"
##init
#i=0
##start of iteration
#while true
#
#do
#((++i))
#echo -e "\nIn iteration" $i
#
##double round brackets performs arithmetic operation and enable you to drop the $
#if ((i>2))
#then
#result=($(./checkConvergence.py))
##echo ${result[0]}
##echo ${result[1]}
#
#if [ "${result[0]}" == "TRUE" ]
#
#then
#echo -e "Increase in locality converges, ending Algorithm"
#break
#fi
#
#fi
#
##run clustered iteration on every 3rd round
#if ((i%3==0))
#then
#echo -e "Running clustered moving round\n"
#./lp_ingredient_producer_clus $FileName $shard > lp_ingred_$i.txt
#
#./linear < lp_ingred_$i.txt | lp_solve | ./clean | sort > x_result_$i.txt
#
#x_file=x_result_$i.txt
#
#./applyMove $FileName $shard $x_file
#
#else
#echo -e "Running individual moving round\n"
#./lp_ingredient_producer_individual $FileName $shard > lp_ingred_$i.txt
#
#./linear < lp_ingred_$i.txt | lp_solve | ./clean | sort > x_result_$i.txt
#
#x_file=x_result_$i.txt
#
#./applyMove $FileName $shard $x_file
#fi
#
#done

#plotting graph after finish looping
#chmod +x graph_plot.py
#./graph_plot.py


