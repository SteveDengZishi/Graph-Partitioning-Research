#!/bin/bash

#  execute_clusterMove.sh
#  Hybrid clustered plus linear programming (HCLP)
#  Boost
#
#  Created by Steve DengZishi on 11/28/19.
#  Copyright © 2019 Steve DengZishi. All rights reserved.
echo -e "\n        Clustered move at Ugandar convergence Version 2.0"
echo -e "       Copyright © 2019 Steve DengZishi  New York University\n"

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

chmod +x checkConvergence.py

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


