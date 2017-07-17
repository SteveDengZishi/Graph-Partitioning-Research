#!/bin/bash

#  executeBLPG_script.sh
#  Boost
#
#  Created by Steve DengZishi on 7/12/17.
#  Copyright © 2017 Steve DengZishi. All rights reserved.

#set the source file for input
read -p "Enter the name of the input graph file: " FileName
read -p "Enter the number of shard: " shard

#compile all .cpp files to executables
g++ -o clean clean.cpp -std=c++11
g++ -o lp_ingredient_producer lp_ingredient_producer.cpp -std=c++11
g++ -o linear linear.cpp -std=c++11
g++ -o RandomAssignment RandomAssignment.cpp -std=c++11

echo -e "\ng++ compilation complete\n"

#execute ReadData with input graph
#prompt user input
#read -p "Enter the number of iteration: " iter

#initialize using random assignment outside of iteration

./RandomAssignment <<END
$FileName
$shard
END

./lp_ingredient_producer > lp_ingred.txt <<END
$FileName
$shard
END

./linear < lp_ingred.txt | lp_solve | ./clean | sort #> x_result_$i.txt

#start of iteration
#for((i=1;i<iter+1;i++))
#do
##vector<int> shard[8], vector<int> adjList[4039], and int prevShard[4039] should not be cleared between iterations
##./lp_ingredient_producer <<EOF $FileName $shard EOF | ./linear > lp_eqns_$i.txt
##lp_solve lp_eqns_$i.txt > x_result_$i.txt
##./applydMove < x_result_$i.txt > movement_$i.txt
##plotting graph along the way
#
#done
