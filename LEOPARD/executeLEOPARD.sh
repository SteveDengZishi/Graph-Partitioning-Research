#!/bin/bash

#  Script.sh
#  LEOPARD
#
#  Created by Steve DengZishi on 7/30/17.
#  Copyright © 2017 Steve DengZishi. All rights reserved.
echo -e "\nLightweight Edge-Oriented Partitioning and Replication for Dynamic Graphs(LEOPARD)  Version 1.0"
echo -e "                    Copyright © 2017 Steve DengZishi  New York University\n"

#set the source file for input
read -p "Enter the file name of the undirected graph: " FileName
read -p "Enter the number of partitions k (k<1000): " shard

#compile all .cpp files to executables
g++ -o LEOPARD_streaming LEOPARD_streaming.cpp -std=c++11
echo -e "\ng++ compiled leopard.cpp successfully"


#initialize using random assignment outside of iteration
echo -e "Starting LEOPARD streaming...\n"
./LEOPARD_streaming <<EOF
$FileName
$shard
EOF
