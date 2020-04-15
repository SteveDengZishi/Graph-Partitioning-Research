#!/bin/bash

#  METIS_BLP.sh
#  Boost
#
#  Created by Steve DengZishi on 7/24/17.
#  Copyright © 2017 Steve DengZishi. All rights reserved.
echo -e "\nMETIS Initialized Balanced Label Propagation      Version 1.0"
echo -e "       Copyright © 2017 Steve DengZishi  New York University\n"
#prompt user for input
read -p "Please enter the fileName: " FileName
read -p "Please enter the number of partition: " partition
read -p "Enter the number of iteration to carry out Balanced Label Propagation after METIS: " iter

#compile all .cpp files to executables
g++ -o METIS_graph METIS_graph.cpp -std=c++11
echo -e "\ng++ compiled METIS_graph.cpp successfully"
g++ -o METIS_format_changer METIS_format_changer.cpp -std=c++11
echo "g++ compiled METIS_format_changer.cpp successfully"
g++ -o clean clean.cpp -std=c++11
echo -e "g++ compiled clean.cpp successfully"
g++ -o lp_ingredient_producer_individual lp_ingredient_producer_individual.cpp -std=c++11
echo "g++ compiled lp_ingredient_producer_individual.cpp successfully"
g++ -o linear linear.cpp -std=c++11
echo "g++ compiled linear.cpp successfully"
g++ -o applyMove applyMove.cpp -std=c++11
echo -e "g++ compiled applyMove.cpp successfully\n"

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

echo -e "Running Balanced Label Propagation on top of METIS......\n"

#start of iteration
for((i=1;i<iter+1;i++))
do
echo "In iteration" $i

./lp_ingredient_producer_individual $FileName $partition > lp_ingred.txt

./linear < lp_ingred.txt > lp_source.txt
lp_solve < lp_source.txt > lp_rawResult.txt
./clean < lp_rawResult.txt | sort > x_result_$i.txt

x_file=x_result_$i.txt

./applyMove $FileName $partition $x_file
done
)

#plotting graph after finish looping
chmod +x graph_plot.py
./graph_plot.py

