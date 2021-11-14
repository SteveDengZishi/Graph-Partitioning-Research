# !/bin/bash
# experimentRunner.sh

fileName=('livejournal.txt' 'orkut.txt')
partitions=(50 100 150 200)
seed=-1

for file in "${fileName[@]}"
do
	for par in "${partitions[@]}"
	do
		echo -e "$file\n$par\n$seed" | ./executeAutoConvergeBLP.sh
	done
done
