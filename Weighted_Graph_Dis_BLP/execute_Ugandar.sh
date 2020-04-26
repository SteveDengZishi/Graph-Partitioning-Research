# !/bin/bash

#  execute_Ugandar.sh
#  In Ugandar proposed linear constraint relocation
#
#  Created by Steve DengZishi on 3/21/18.
#  Copyright © 2018 Steve DengZishi. All rights reserved.
echo -e "\nIn Ugandar proposed linear constraint relocation"
echo -e "Copyright © 2017 Steve DengZishi  New York University\n"

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
g++ -o lp_ingredient_producer lp_ingredient_producer_individual.cpp -std=c++11
echo "g++ compiled lp_ingredient_producer.cpp successfully"
g++ -o linear linear.cpp -std=c++11
echo "g++ compiled linear.cpp successfully"
g++ -o applyMove applyMove.cpp -std=c++11
echo "g++ compiled applyMove.cpp successfully"

chmod +x checkConvergence.py

#time the effective execution
time(
#to make sure do not run two disruptive rounds in a row
i=0
#start of iteration
while true

do
((++i))
echo -e "\nIn iteration" $i

#after the first two rounds, start to check whether result converges
if (($i>2))

then
#declare array to store the returned values
result=($(./checkConvergence.py))

#echo ${result[0]}
#echo ${result[1]}

if [ "${result[0]}" == "TRUE" ]

then
echo "Increase in locality converges"
echo "Ending BLP as result converges"

echo "The highest locality is: ${result[1]}"
echo "($FileName, $shard, ${result[1]})" >> output/result_data.txt

break

else
#taking too much time for large graph
./lp_ingredient_producer $FileName $shard > lp_ingred.txt

./linear < lp_ingred.txt > lp_source.txt
lp_solve < lp_source.txt > lp_rawResult.txt
./clean < lp_rawResult.txt | sort > x_result_$i.txt

x_file=x_result_$i.txt
#too much time for large graph, map to move large time for many partitions
./applyMove $FileName $shard $x_file

fi

else
./lp_ingredient_producer $FileName $shard > lp_ingred.txt

./linear < lp_ingred.txt > lp_source.txt
lp_solve < lp_source.txt > lp_rawResult.txt
./clean < lp_rawResult.txt | sort > x_result_$i.txt

x_file=x_result_$i.txt
#too much time for large graph, map to move large time for many partitions
./applyMove $FileName $shard $x_file

fi

done
)
