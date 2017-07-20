#!/bin/bash
read -p "Enter the name of the input graph file: " FileName
read -p "Enter the number of shard: " shard
#prompt user input
#read -p "Enter the number of iteration: " iter

#compile all .cpp files to executables
g++ -o clean clean.cpp -std=c++11
echo -e "\ng++ compiled clean.cpp successfully"
g++ -o lp_ingredient_producer lp_ingredient_producer.cpp -std=c++11
echo "g++ compiled lp_ingredient_producer.cpp successfully"
g++ -o linear linear.cpp -std=c++11
echo "g++ compiled linear.cpp successfully"
g++ -o applyMove applyMove.cpp -std=c++11
echo "g++ compiled applyMove.cpp successfully"
g++ -o RandomAssignment RandomAssignment.cpp -std=c++11
echo -e "g++ compiled RandomAssignment.cpp successfully\n"


#initialize using random assignment outside of iteration
echo -e "Starting random initialization...\n"
./RandomAssignment <<EOF
$FileName
$shard
EOF

echo -e "Initialization completed\n"

./lp_ingredient_producer <<EOF
$FileName
$shard
EOF
