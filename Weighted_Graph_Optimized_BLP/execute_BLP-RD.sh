# !/bin/bash

#  execute_BLP-RD.sh
#  Boost
#
#  Created by Steve DengZishi on 3/21/18.
#  Copyright © 2018 Steve DengZishi. All rights reserved.

echo -e "    Probability Based Selective Ratio Disruption with Replication"
echo -e "       Copyright © 2017 Steve DengZishi  New York University\n"

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
echo -e "\ng++ compiled clean.cpp successfully"
g++ -o lp_ingredient_producer_individual lp_ingredient_producer_individual.cpp -std=c++11
echo "g++ compiled lp_ingredient_producer.cpp successfully"
g++ -o linear linear.cpp -std=c++11
echo "g++ compiled linear.cpp successfully"
g++ -o applyMove applyMove.cpp -std=c++11
echo "g++ compiled applyMove.cpp successfully"
g++ -o probDisruptiveMove probDisruptiveMove.cpp -std=c++11
echo "g++ compiled disruptiveMove.cpp successfully"
chmod +x checkConvergence.py

#time the effective execution
time(

#to make sure do not run two disruptive rounds in a row
skip=0
last=0
i=0
th2=0.01
#start of iteration
while true

do
((++i))
echo "In iteration" $i

#after the first two rounds, start to check whether result converges
if (($i>2)) && (($skip==0))

then
#declare array to store the returned values
result=($(./checkConvergence.py))

#echo ${result[0]}
#echo ${result[1]}

if [ "${result[0]}" == "TRUE" ]

then
echo "Increase in locality converges"
#echo $last

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
echo "Disruptive condition met, running disruptive round"
alpha=$(echo "50/$i" | bc -l)
#echo $alpha
./probDisruptiveMove $FileName $shard $seed $alpha

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
#taking too much time for large graph
./lp_ingredient_producer_individual $FileName $shard > lp_ingred.txt

./linear < lp_ingred.txt | lp_solve | ./clean | sort > x_result_$i.txt

x_file=x_result_$i.txt
#too much time for large graph, map to move large time for many partitions
./applyMove $FileName $shard $x_file

skip=0
fi

else
./lp_ingredient_producer_individual $FileName $shard > lp_ingred.txt

./linear < lp_ingred.txt | lp_solve | ./clean | sort > x_result_$i.txt

x_file=x_result_$i.txt

./applyMove $FileName $shard $x_file

skip=0
fi

done
)

#plotting graph after finish looping
chmod +x graph_plot.py
./graph_plot.py
