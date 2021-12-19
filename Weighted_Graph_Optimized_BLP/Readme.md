**Shell Scripts are entry points of the programs**
	
	experiment_runner.sh
		./experimentRunner (init_method *) (refinement_disruption_method *) (graphFile) (partition_num)
		takes 4 arguments with * as mandantory, graphFile and partition_num will be prompt for input if not given in command line args
	experiment_Batch_Generator.sh
		You can modify script to give a list of files and a list of partition number to generate comparison on a certain combination of init and refinement disruption method to generate comparison results. It loops through all args and calls experiment_runner.sh under the hood.
