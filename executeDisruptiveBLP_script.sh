#!/bin/bash
#  executeBLPG_script.sh
#  Boost
#
#  Created by Steve DengZishi on 7/12/17.
#  Copyright © 2017 Steve DengZishi. All rights reserved.
echo -e "\nRandomly Initialized Balanced Label Propagation      Version 1.1"
echo -e "       Copyright © 2017 Steve DengZishi  New York University\n"

#set the source file for input
read -p "Enter the file name of the undirected graph: " FileName
read -p "Enter the number of partitions k (k<1000): " shard
#prompt user input
read -p "Enter the number of iteration to carry out Balanced Label Propagation: " iter

#compile all .cpp files to executables
g++ -o clean clean.cpp -std=c++11
echo -e "\ng++ compiled clean.cpp successfully"
g++ -o lp_ingredient_producer lp_ingredient_producer.cpp -std=c++11
echo "g++ compiled lp_ingredient_producer.cpp successfully"
g++ -o linear linear.cpp -std=c++11
echo "g++ compiled linear.cpp successfully"
g++ -o applyMove applyMove.cpp -std=c++11
echo "g++ compiled applyMove.cpp successfully"
g++ -o disruptiveMove disruptiveMove.cpp -std=c++11
echo "g++ compiled disruptiveMove.cpp successfully"
g++ -o RandomAssignment RandomAssignment.cpp -std=c++11
echo -e "g++ compiled RandomAssignment.cpp successfully\n"
chmod +x checkConvergence.py

#time the effective execution
time(
#initialize using random assignment outside of iteration
echo -e "Starting random initialization...\n"
./RandomAssignment <<EOF
$FileName
$shard
EOF

echo -e "Initialization completed\n"

#to make sure do not run two disruptive rounds in a row
skip=0
#start of iteration
for((i=1;i<iter+1;i++))

do
    echo "In iteration" $i

    #after the first two rounds, start to check whether result converges
    if (($i>2)) && (($skip==0))

    then
        result=$(./checkConvergence.py)
        #echo $result
        if [ "$result" == "TRUE" ]

        then
            echo "Increase in locality converges. Running disruptive round"
            ./disruptiveMove <<EOF
            $FileName
            $shard
EOF
            skip=1

        else
            ./lp_ingredient_producer > lp_ingred.txt <<EOF
            $FileName
            $shard
EOF

            ./linear < lp_ingred.txt | lp_solve | ./clean | sort > x_result_$i.txt

            x_file=x_result_$i.txt

            ./applyMove <<EOF
            $FileName
            $shard
            $x_file
EOF
            skip=0
        fi

    else
        ./lp_ingredient_producer > lp_ingred.txt <<EOF
        $FileName
        $shard
EOF

        ./linear < lp_ingred.txt | lp_solve | ./clean | sort > x_result_$i.txt

        x_file=x_result_$i.txt

        ./applyMove <<EOF
        $FileName
        $shard
        $x_file
EOF
        skip=0
    fi

done

)

#plotting graph after finish looping
chmod +x graph_plot.py
./graph_plot.py
