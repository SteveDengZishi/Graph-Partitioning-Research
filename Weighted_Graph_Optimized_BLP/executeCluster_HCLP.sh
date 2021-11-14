#!/bin/bash

#  Cluster_HCLP.sh
#  Hybrid clustered plus linear programming (HCLP)
#  With pairwise partitions swapping at the end of convergence
#  Boost
#
#  Created by Steve DengZishi on 11/28/19.
#  Copyright © 2019 Steve DengZishi. All rights reserved.
echo -e "\nClustered Prob-Disruptive Balanced Label Propagation  Version 2.0"
echo -e "       Copyright © 2019 Steve DengZishi  New York University\n"
#prompt user for input
read -p "Please enter the fileName: " FileName
read -p "Please enter the number of partition k (k<1000): " shard
read -p "Enter the seed of random probability: " seed
read -p "Enter 'v' to verbose clustering movement outputs or press enter to skip: " verbose

#compile all .cpp files to executables
g++ -o clus clus.cpp -std=c++11
echo -e "\ng++ compiled clus.cpp successfully"
g++ -o greedyAssignment greedyAssignment.cpp -std=c++11
echo -e "g++ compiled greedyAssignment.cpp successfully"
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
g++ -o applyMove_clus applyMove_clus.cpp -std=c++11
echo -e "g++ compiled applyMove_clus.cpp successfully"
g++ -o probDisruptiveMove probDisruptiveMove.cpp -std=c++11
echo -e "g++ compiled disruptiveMove.cpp successfully"
g++ -o pairwise_partition_swap pairwise_partition_swap.cpp -std=c++11
echo -e "g++ compiled pairwise_partition_swap.cpp successfully\n"
chmod +x checkConvergence.py

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
#init
skip=0
last=0
i=0
th2=0.001
#start of iteration
while true

do
((++i))
echo -e "\nIn iteration" $i

#double round brackets performs arithmetic operation and enable you to drop the $
if ((i>2)) && ((skip==0))
then
result=($(./checkConvergence.py))
#echo ${result[0]}
#echo ${result[1]}

if [ "${result[0]}" == "TRUE" ]

then
echo -e "Increase in locality converges"

if(( $(echo "$last <= 0" | bc -l) ))
then
#the new peak improvement percentage as compared to the prior peak
converge_imp_ratio=1
else
converge_imp_ratio=$(echo "(${result[1]} - $last) / $last" | bc -l)
fi
#echo $converge_imp_ratio


if (( $(echo "$converge_imp_ratio > $th2" | bc -l) ))

then
echo "Disruptive condition met, Running clustered moving round"

./lp_ingredient_producer_clus $FileName $shard > lp_ingred.txt

./linear < lp_ingred.txt > lp_source.txt
lp_solve < lp_source.txt > lp_rawResult.txt
./clean < lp_rawResult.txt | sort > x_result_$i.txt

x_file=x_result_$i.txt

./applyMove_clus $FileName $shard $x_file $seed

skip=1

last=${result[1]}

else
echo -e "Converges, ending Balanced Label Propagation"
if (( $(echo "$last > ${result[1]}" | bc -l) ))
then
echo "The highest locality is: $last"
else
echo "The highest locality is: ${result[1]}"
fi

break

fi

else
echo -e "Running individual moving round\n"
./lp_ingredient_producer_individual $FileName $shard > lp_ingred.txt

./linear < lp_ingred.txt > lp_source.txt
lp_solve < lp_source.txt > lp_rawResult.txt
./clean < lp_rawResult.txt | sort > x_result_$i.txt

x_file=x_result_$i.txt

./applyMove $FileName $shard $x_file

skip=0
fi

else
echo -e "Running individual moving round\n"
./lp_ingredient_producer_individual $FileName $shard > lp_ingred.txt

./linear < lp_ingred.txt > lp_source.txt
lp_solve < lp_source.txt > lp_rawResult.txt
./clean < lp_rawResult.txt | sort > x_result_$i.txt

x_file=x_result_$i.txt

./applyMove $FileName $shard $x_file

skip=0
fi

done

#plotting graph after finish looping
chmod +x graph_plot.py
./graph_plot.py


