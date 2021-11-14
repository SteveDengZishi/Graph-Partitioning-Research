directories=('athletes_edges_clean' 'company_edges_clean' 'email-Enron' 'livejournal' 'orkut' 'roadNetCA')

for directory in ${directories[@]}
	do cd $directory; pwd;
	for f in *.txt; 
		do 
			mv "$f" "$(echo "$f" | sed 's/commGreedy/SBM/;s/random/Random/;s/gpmetis/Metis/;s/Ugandar/BLP/;s/clusterMove/BLP-MC/;s/pairwiseSwap/BLP-KL/')";
	done
	cd ../
done