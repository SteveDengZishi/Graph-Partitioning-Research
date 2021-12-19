                      Paper Name: Optimizing Iterative Algorithms for Social Network Sharding
                                    Published Conference: IEEE Big Data 2021

	           Student: Zishi Deng    Supervisor: Torsten Suel  Institution: New York University
      
**Research Abstract**
There has recently been significant interest in applications that require computations on massive graph structures, including scenarios where the graph is too large to be processed on a single machine. In this case, the graph needs to be partitioned into subgraphs that can be assigned to individual machines, in a process called graph or social network sharding. Given the sizes of the graphs involved, it is necessary or at least highly desirable that the partitioning itself can also be performed in a distributed manner, instead of running a sequential partitioning algorithm on a single node.

We study such distributed algorithms for graph sharding, where the goal is to create subgraphs of roughly equal size that minimize the number of edges crossing subgraph boundaries. In particular, we focus on two well-known approaches that can be efficiently implemented in MapReduce and related distributed computing paradigms: the Balanced Label Propagation algorithm of Ugander and Backstrom, and the method of Duong et al. based on the Bayesian Stochastic Block Modeling approach of Hofman and Wiggins. Our contributions are as follows: (1) We perform the first direct experimental comparison of the two approaches, which were independently proposed and published. (2) We propose and evaluate several enhancements of Balanced Label Propagation that result in improved graph shardings. (3) We propose and evaluate hybrid methods that perform label propagation both on individual nodes, as suggested by Ugander and Backstrom, and on stochastic blocks inferred using the approach of Duong et al.


**Repository Description**

	This git repository consists of five folders

	Balanced_Label_Propagation_Deprecated
	Community_Detection
	Disruptive_BLP_AutoConverge
	References
	**Weighted_Graph_Optimized_BLP**
	
NOTE THAT: **Weight_Graph_Optimized_BLP** folder contains **all up-to-date implementations** for initializations, refinement and disruption techniques, including BLP, SBM, Probability based disruption, cluster based movement, KL Swaps, you should **use these up-to-date implementations**

	Balanced_Label_Propagation consists of two parts, it was the first implementation but it is DEPRECATED!
	1.Randomly initialized Balanced Label Propagation
	2.METIS + BLP (Balanced Label Propagation run on top of METIS)

	Disruptive_BLP_AutoConverge is the major improvements from Balanced_Label_Propagation
	1.Improved mapping algorithm allow it to run faster on larger graph and partitions
	2.It will run disruptive rounds when it is close to convergence (When improvement in locality between two rounds < 0.5%)
	3.It will automatically stop the BLP process and return the highest locality achieved when no improvement can be achieved (Convergence arises and it is lower than the previous convergence point)
	
	Community_Detection contains SBM methods for clustering and greedy map to blocks

	Weighted_Graph_Optimized_BLP contains all up-to-date algorithms used in generating results in the paper as well as helper script to run multiple comparison with plotting. 

	References contains all the relevant papers referred during this research 
	

**Use Instruction**

	Please go into the respective sub-folders to read the instructions from there
