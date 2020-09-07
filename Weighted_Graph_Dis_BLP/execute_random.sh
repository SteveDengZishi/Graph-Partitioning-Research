# !/bin/bash

#  execute_Random.sh
#  Boost
#
#  Created by Steve DengZishi on 3/21/18.
#  Copyright © 2018 Steve DengZishi. All rights reserved.
echo -e "\n              In Random Initialization"
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
g++ -o randomAssignment randomAssignment.cpp -std=c++11
echo -e "g++ compiled randomAssignment.cpp successfully"

#initialize using random assignment outside of iteration
echo -e "Starting random initialization...\n"
time(
./randomAssignment $FileName $shard $seed
)

echo -e "Finished Random initialization...\n"
