#!/bin/bash

#  Cluster_HCLP.sh
#  Hybrid clustered plus linear programming (HCLP)
#  Boost
#
#  Created by Steve DengZishi on 1/7/19.
#  Copyright © 2019 Steve DengZishi. All rights reserved.
echo -e "\nClustered Non-Greedy Prob-Disruptive Balanced Label Propagation  Version 1.7"
echo -e "       Copyright © 2019 Steve DengZishi  New York University\n"
#prompt user for input
read -p "Please enter the fileName: " FileName
read -p "Please enter the number of partition k (k<1000): " shard
read -p "Enter the seed of random probability: " seed
read -p "Enter 'v' to verbose clustering movement outputs or press enter to skip: " verbose

#compile all .cpp files to executables
g++ -o clus clus.cpp -std=c++11
echo -e "\ng++ compiled clus.cpp successfully"
g++ -o simpleClusterAssignment simpleClusterAssignment.cpp -std=c++11
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
echo -e "g++ compiled disruptiveMove.cpp successfully\n"
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
echo -e "Starting serpentine initialization...\n"
./simpleClusterAssignment $FileName $shard $verbose

echo -e "Initialization completed\n"
#init
individualMove=0
i=0
th2=0.0005
#start of iteration
while true
    do
    ((++i))
    echo -e "\nIn iteration" $i

    #double round brackets performs arithmetic operation and enable you to drop the $
    #after the first 2 rounds to start checking convergence, no longer need to skip consecutive rounds
    if ((i>2))
        then
        result=($(./checkConvergence.py))
    fi
    
    if [ "${result[0]}" == "FALSE" ] || [ $individualMove -eq 0 ]
        then
        echo -e "Running clustered moving round\n"
        
        ./lp_ingredient_producer_clus $FileName $shard > lp_ingred_$i.txt

        ./linear < lp_ingred_$i.txt | lp_solve | ./clean | sort > x_result_$i.txt

        x_file=x_result_$i.txt

        ./applyMove_clus $FileName $shard $x_file $seed


    elif [ $individualMove -eq 1 ] && [ "${result[0]}" == "TRUE" ]
        then
        echo -e "Converges, ending Balanced Label Propagation"
        
        echo "The highest locality is: ${result[1]}"
        break



    else
        echo -e "Increase in locality converges for clustered movements, moving individual rounds onwards"
        individualMove=1
        
        ./lp_ingredient_producer_clus $FileName $shard > lp_ingred_$i.txt

        ./linear < lp_ingred_$i.txt | lp_solve | ./clean | sort > x_result_$i.txt

        x_file=x_result_$i.txt

        ./applyMove_clus $FileName $shard $x_file $seed
        
    fi

done

#plotting graph after finish looping
chmod +x graph_plot.py
./graph_plot.py


