# !/bin/bash
# experimentRunner.sh

#fileName=('athletes_edges_clean.txt' 'orkut.txt')
fileName=('company_edges_clean.txt' 'livejournal.txt')
partitions=(10 30 50 70 90)
initMethod=('commGreedy' 'random' 'gpmetis')
imprvMethod=('Ugandar' 'clusterMove' 'pairwiseSwap')
seed=-1

for file in ${fileName[@]}
do
	for par in ${partitions[@]}
	do
        for init in ${initMethod[@]}
        do
            for imprv in ${imprvMethod[@]}
            do
                echo "Running experimentRunner.sh ${init} ${imprv} ${file} ${par}"
                ./experimentRunner.sh $init $imprv $file $par
            done
        done
	done
    IFS="." read -ra files <<< "$file"
    cd output
    mkdir "${files[0]}"
    mv graph_plotting_*"${files[0]}"*.txt "${files[0]}"/
    mv sharding_result_*"${files[0]}"*.bin "${files[0]}"/
    cd ../
done
