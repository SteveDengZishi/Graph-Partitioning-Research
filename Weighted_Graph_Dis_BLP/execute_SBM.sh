#!/bin/bash

#  execute_SBM.sh
#  Community Clustering and Greedy Mappping to Partitions (Sharding Social Network) Initialization
#
#  Created by Steve DengZishi on 11/28/19.
#  Copyright © 2019 Steve DengZishi. All rights reserved.
echo -e "\nIn Community Clustering and Greedy Mappping to Partitions (Sharding Social Network) Initialization"
echo -e "Copyright © 2019 Steve DengZishi  New York University\n"

if [ "$#" -eq 4 ]; then

verbose=$4

elif [ "$#" -eq 3 ]; then

verbose=''

else
echo "Wrong number of args provided: Use Case 1.Filename 2.partitions 3.seed 4.verbose(optional)"
exit 1
fi

FileName=$1
shard=$2
seed=$3

#compile all .cpp files to executables
g++ -o clus clus.cpp -std=c++11
echo -e "\ng++ compiled clus.cpp successfully"
g++ -o greedyAssignment greedyAssignment.cpp -std=c++11
echo -e "g++ compiled greedyAssignment.cpp successfully"

chmod +x checkConvergence.py

IFS="." read -ra file <<< "$FileName"
clusterFile="${file[0]}_clusters.txt"
echo -e "Searching for clustered community file: $clusterFile\n"

if [ ! -f "$clusterFile" ]
then
echo -e "Not found previously generated clusters file ${clusterFile}\n"
echo -e "Running Bayesian Clustering Algorithm...\n"
./clus $FileName $seed $verbose
else
echo -e "Skipping Clustering as file exists...\n"
fi

#Random initialization with clusters
echo -e "Starting greedy initialization using community clusters...\n"
./greedyAssignment $FileName $shard $verbose

echo -e "Initialization completed\n"
