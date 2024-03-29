#!/bin/bash

#  experimentRunner.sh
#
#  Created by Steve Deng Zishi on 01/07/20.
#  Copyright © 2019 Steve DengZishi. All rights reserved.

echo -e "\n           ExperimentRunner Version 4.0"
echo -e "Copyright © 2019 Steve DengZishi  New York University\n"

initMethod=$1
impMethod=$2

if [ "$#" -eq 2 ]
then
#prompt user for input
read -p "Please enter the fileName: " FileName
read -p "Please enter the number of partition k (k<1000): " shard
read -p "Enter the seed of random probability: " seed
read -p "Enter 'v' to verbose clustering movement outputs or press enter to skip: " verbose

elif [ "$#" -eq 4 ]
then
FileName=$3
shard=$4
seed=-1
verbose=''

else
echo -e "illegal number of arguments, please check use cases and give two or four arguments as below, * are mandantory\n"
echo -e "./experimentRunner (init_method *) (improvement_method *) (graphFile) (partition_num)\n"
echo "Available init method list: [random -> Random, commGreedy -> SBM, gpmetis -> Metis]"
echo -e "Available improvement/refinement method list: [Ugandar_BLP -> BLP, clusterMove -> BLP-MC, KL_Swap -> BLP-KL]\n"
exit 1
fi

#cleaning up all past execution intermediate files
rm -rf lp_ingred*.txt
rm -rf x_result*.txt
rm -rf metis.graph*

chmod +x execute_"$initMethod".sh
chmod +x execute_"$impMethod".sh

#initialization method
./execute_"$initMethod".sh $FileName $shard $seed $verbose

if [ $? -gt 0 ] ; then
     echo "Execution of initialization method has failed please check"
     echo "exiting...."
     exit 1
fi

#improvement/refinement method
./execute_"$impMethod".sh $FileName $shard $seed $verbose

if [ $? -gt 0 ] ; then
     echo "Execution of improvement method has failed please check"
     echo "exiting...."
     exit 1
fi

#plotting graph after finish looping
chmod +x graph_plot.py
./graph_plot.py

IFS="." read -ra file <<< "$FileName"
cp graph_plotting_data.txt output/graph_plotting_"$initMethod"_"$impMethod"_"${file[0]}"_"$shard".txt
cp sharding_result.bin output/sharding_result_"$initMethod"_"$impMethod"_"${file[0]}"_"$shard".bin



