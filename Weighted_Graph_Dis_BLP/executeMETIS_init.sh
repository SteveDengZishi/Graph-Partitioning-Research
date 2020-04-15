#!/bin/bash

#  METIS_BLP.sh
#  Boost
#
#  Created by Steve DengZishi on 7/24/17.
#  Copyright © 2017 Steve DengZishi. All rights reserved.
echo -e "\nIn METIS Initialization"
echo -e "Copyright © 2017 Steve DengZishi  New York University\n"

if ["$#" -ne 4]; then
echo "Wrong number of args provided: Use Case 1.Filename 2.partitions 3.seed 4.verbose"
exit 1
fi

#prompt user for input
FileName = $1
partition = $2
seed = $3
verbose = $4

#compile all .cpp files to executables
g++ -o METIS_graph METIS_graph.cpp -std=c++11
echo -e "\ng++ compiled METIS_graph.cpp successfully"
g++ -o METIS_format_changer METIS_format_changer.cpp -std=c++11
echo "g++ compiled METIS_format_changer.cpp successfully"

echo -e "Running METIS......\n"

#time the effective execution
time(
#change input graph file from edge list format into METIS input format
./METIS_graph > metis.graph <<EOF
$FileName
EOF

#run gpmetis to partition graph into k partitions
gpmetis metis.graph $partition

#1.change the METIS output to standard BLP input format
#2.output locality info and save them for bokeh plotting
./METIS_format_changer <<EOF
$FileName
$partition
EOF

echo -e "Finished METIS Initialization......\n"
