**Shell Scripts are entry points of the programs**
	
	experiment_runner.sh
		./experimentRunner.sh (init_method *) (refinement_disruption_method *) (graphFile) (partition_num)
		takes 4 arguments with * as mandantory, graphFile and partition_num will be prompt for input if not given in command line args
		List of provided initialization methods - [Random, Metis, SBM]
		List of provided refinement + disruption methods - [BLP, BLP-RD, BLP-MC, BLP-KL]

		An example call will be:
		./experimentRunner.sh SBM BLP-MC dataset/facebook_combined.txt 10 

	experiment_Batch_Generator.sh
		You can modify script to give a list of files and a list of partition number
		to generate comparison on a certain combination of init and refinement disruption methods
		It loops through all args and calls experiment_runner.sh under the hood.

**Input Graph File format**
	
	we provide a small example test file in dataset folder, but you should source your own dataset
		facebook_combined.txt
		It starts with a header line consist the number of node in the graph the and number of undirected edges in a graph
		then on each line represent an undirected edge with from-nodeID to to-nodeID
		Note that nodeID must be cleaned from 0 to (number of node - 1)

**Dependencies needed before you execute the program**

	g++ we used g++ to compile all c++ code to executables
	python3 we used python for graph plotting
	lp_solve it should be installed and callable from shell script to solve linear programs 
