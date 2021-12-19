**Shell Scripts are entry points of the programs**
	
	experiment_runner.sh
		./experimentRunner (init_method *) (refinement_disruption_method *) (graphFile) (partition_num)
		takes 4 arguments with * as mandantory, graphFile and partition_num will be prompt for input if not given in command line args
		List of provided initialization methods - [Random, Metis, SBM]
		List of provided refinement + disruption methods - [BLP, BLP-RD, BLP-MC, BLP-KL]

	experiment_Batch_Generator.sh
		You can modify script to give a list of files and a list of partition number
		to generate comparison on a certain combination of init and refinement disruption methods
		It loops through all args and calls experiment_runner.sh under the hood.

**Input Graph File format**
	we provide a small example test file in dataset folder
	facebook_combined.txt
