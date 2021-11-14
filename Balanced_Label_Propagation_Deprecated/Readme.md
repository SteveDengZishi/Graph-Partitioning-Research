**Use Instruction**

	1.To use Randomly initialized Balanced Label Propagation with bokeh plotting:
		
		chmod +x executeBLP_script.sh
		./executeBLP_script.sh

		following the prompt to enter the fileName
			the first line of the file must be number of nodes and edges
			the file then consist all edges from nodeID to nodeID one at a line

		enter the number of partitions k (k<1000)

		enter the number of iterations of BLP that you intend to perform on graph

		then the bash script will compile all required cpp files and do everything



	2.To use METIS + BLP, running BLP on top of METIS

		chmod +x executeMETIS_BLP.sh
		./executeMETIS_BLP.sh
	
		following the prompt to enter the fileName
			the first line of the file must be number of nodes and edges
			the file then consist all edges from nodeID to nodeID one at a line

		enter the number of partitions k (k<1000)

		enter the number of iterations of BLP that you intend to perform on graph
		on top of METIS result

		then the bash script will compile all required cpp files and do everything