//
//  Clustering.cpp
//  Boost
//
//  Created by Steve DengZishi on 9/13/18.
//  Copyright © 2018 Steve DengZishi. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cmath> // To use sqrt()
#include <list> // To use STL linked-list in dfs topological sort
#include <stack> // To use STL stack in topological sort
#include <queue> // To use STL queue in bfs, dijkstra's, maxflow
#include <cstring> // To use memset()
#include <cstdlib> // To use atoi(), abs(), malloc()
#include <algorithm> // To use sort(), next_permutation(), min(), max() etc.
#include <unordered_map> // To allow O(1) mapping access using key->value
#include <set> // To sort and remove duplicate when inserted
#include <unordered_set> //To remove duplicates and count size
#include <functional> //To use std::hash
#include <fstream> // To use c++ input and output to files
#include <time.h>
#include <stdlib.h>
#include <boost/math/special_functions/digamma.hpp>

using namespace std;

#define FOR(i,a,b) for(size_t i=a;i<b;i++)
#define DIGAMMA boost::math::digamma
//global variables here
vector<int>* blocks;
vector<int> blockSize;
//check whether it is initialized
int* prevShard;
vector<int>* adjList;
//int** adjMatrix;
//double* pi_weights;
fstream inFile;
string fileName;
int block_num;
int nodes;
int edges;
int iterations;
double J;
double JL;
double* h;
string verbose;

void randomAssignment(){
    srand(time(NULL));
    //random sharding according using a integer hash then mod 8 to distribute to shards
    for(int i=0;i<nodes;i++){
        //        cout<<"i is: "<<i<<endl;
        int block_assignment = rand() % block_num;
        //        cout<<"shard ID is: "<<shardID<<endl;
        blocks[block_assignment].push_back(i);
        //mark prevShard for the 1st random Sharding
        prevShard[i] = block_assignment; 
    }
}

double printLocatlityFraction(){
    int localEdge=0;
    FOR(i,0,nodes){
        FOR(j,0,adjList[i].size()){
            if(prevShard[i]==prevShard[adjList[i][j]]) localEdge++;
        }
    }
    localEdge/=2; //if the graph is undirected each edge was counted twice
    int totalEdge=edges;
    double fraction=(double)localEdge/(double)totalEdge;
    printf("\nThere are %d local edges out of total %d edges, fraction of local edges is: %lf\n",localEdge,totalEdge,fraction);
    return fraction;
}

//build adjacency list from streaming edges
void createADJ(){
    //create data structures with variable size on the heap
    adjList = new vector<int>[nodes];

    int from,to;
    while(inFile>>from>>to){
        adjList[from].push_back(to);
        // comment out the following line if the graph is directed
        adjList[to].push_back(from);
    }
}

//convert adjacency list to adjacency matrix
//it is probably too costly on large graphs
/*
void convertADJtoADJMatrix(){
    //dynamically initialize 2d array to 0s
    adjMatrix = new int*[nodes];

    FOR(i, 0, nodes){
        adjMatrix[i] = new int[nodes]{0};
    }

    //scan through adjList and update edges to matrix
    FOR(i,0,nodes){
        FOR(j,0,adjList[i].size()){
            adjMatrix[i][adjList[i][j]]=1;
        }
    }
}
*/
//replace adjMatrix using adjList in checking edge connection
int check_connection(int i, int j){
    FOR(k,0,adjList[i].size()){
        if(adjList[i][k] == j) return 1;
    }
    return 0;
}
//m++, c+
//deprecated as it is too slow for large graphs to use O(V^2+E) method
/* int countEdgesWithinComm(){
    int count = 0;
    FOR(i,0,nodes){
        FOR(j,0,nodes){
            //printf("%d %d ", i,j);
            if(i>j){
                if(prevShard[i]==prevShard[j] && check_connection((int)i,(int)j)==1){
                    count++;
                }
            }
        }
    }
    cout<<"Total number of edges within communities are: "<<count<<endl;
    return count;
} */

//SUM from 1 to K |E(Bk, Bk)|
int countEdgesWithinComm(){
    int count=0;
    FOR(i,0,block_num){
        FOR(j,0,blocks[i].size()){
            FOR(k,0,blocks[i].size()){
                //undirected graph only count in one direction
                if(blocks[i][j]>blocks[i][k]){
                    if(check_connection(blocks[i][j], blocks[i][k])) count++;
                }
            }
        }
    }
    cout<<"Total number of edges within communities are: "<<count<<endl;
    return count;
}

//m+-, c-
/* int countNonEdgesWithinComm(){
    int count = 0;
    FOR(i,0,nodes){
        FOR(j,0,nodes){
            if(i>j){
                if(prevShard[i]==prevShard[j] && check_connection((int)i,(int)j)==0){
                    count++;
                }
            }
        }
    }
    cout<<"Total number of non-edges within communities are: "<<count<<endl;
    return count;
} */

//SUM from 1 to k (nk(nk-1)/2) - m++
int countNonEdgesWithinComm(int mpp){
    int count=0;
    FOR(i,0,block_num){
        count += blockSize[i]*(blockSize[i]-1)/2;
    }
    count=count-mpp;
    cout<<"Total number of non-edges within communities are: "<<count<<endl;
    return count;
}

//m-+, d+
//deprecated
/* int countEdgesBetweenComm(){
    int count = 0;
    FOR(i,0,nodes){
        FOR(j,0,nodes){
            if(i>j){
                if(prevShard[i]!=prevShard[j] && check_connection((int)i,(int)j)==1){
                    count++;
                }
            }
        }
    }
    cout<<"Total number of edges between communities are: "<<count<<endl;
    return count;
} */

//m-m++
int countEdgesBetweenComm(int mpp){
    int count = edges-mpp;
    cout<<"Total number of edges between communities are: "<<count<<endl;
    return count;
}

//m--, d-.
//deprecated
/* int countNonEdgesBetweenComm(){
    int count = 0;
    FOR(i,0,nodes){
        FOR(j,0,nodes){
            if(i>j){
                if(prevShard[i]!=prevShard[j] && check_connection((int)i,(int)j)==0){
                    count++;
                }
            }
        }
    }
    cout<<"Total number of non-edges between communities are: "<<count<<endl;
    return count;
} */

//n(n-1)/2-m-m-+
int countNonEdgesBetweenComm(int mmp){
    int count = nodes*(nodes-1)/2-edges-mmp;
    cout<<"Total number of non-edges between communities are: "<<count<<endl;
    return count;
}

//nµ, nk
void countBlockSize(){
    blockSize.clear();
    FOR(i,0,block_num){
        blockSize.push_back(blocks[i].size());
    }
}

void printBlockSize(){
    cout<<"\nPrinting blocks sizes:\n";
    FOR(i,0,block_num){
        cout<<blockSize[i]<<" ";
    }
    cout<<endl;
}

/* void generate_pi_weights(){
    int sum = 0;
    srand(time(NULL));
    for(int i=0;i<block_num;i++){
        pi_weights[i] = rand()%1000;
        sum+=pi_weights[i];
    }
    for(int i=0; i<block_num; i++){
        pi_weights[i]/=sum;
    }
}

void print_pi_weights(){
    cout<<"The pi_weights are: "<<endl;
    for(int i=0;i<block_num;i++){
        cout<<pi_weights[i]<<" ";
    }
    cout<<endl<<endl;
}

int get_block_assignment(){
    //generate a random double between 0~1
    double r = (double) rand()/(RAND_MAX);
    //printf("r number is: %f\n",r);
    double pi_sum=0.0;
    for(int i=0;i<block_num;i++){
        pi_sum+=pi_weights[i];
        if(pi_sum>=r){
            return i;
        }
    }
    return block_num;
} */

void print_blocks_assignments(){
    for(int i=0;i<block_num;i++){
        cout<<"In block "<<i<<endl;
        for(int j=0;j<blocks[i].size();j++){
            cout<<blocks[i][j]<<" ";
        }
        cout<<endl;
    }
}

//count the number of edges between node y and block z
int countEdgesBetweenNodeAndBlock(int y, int z){
    int count = 0;
    //each of the nodes in block[z] check it adjMatrix with node y whether edge exists
    FOR(i,0,blocks[z].size()){
        int node = blocks[z][i];
        if(check_connection(y,node)) count++;
    }
    return count;
}

//using discounted vote to select the most probable block assignment for each node
int findBestAssignmentK(int i,double J, double JL, double* h){
    vector<double> results;
    FOR(j,0,block_num){
        //count wk = |E(i,Bk)| + |E(Bk,i)|
        int w_k = countEdgesBetweenNodeAndBlock(i,(int)j);
        //weigh on the block sizes
        int effective_size_k = blockSize[j];
        if(prevShard[i]==(int)j) effective_size_k = blockSize[j] - 1;

        double val = J*w_k - JL*effective_size_k - h[j];
        results.push_back(val);
    }
    //return the max resulting index of the results
    int max_idx=0;
    FOR(j,1,block_num){
        if(results[j] > results[max_idx]) max_idx=j;
    }
    return max_idx;
}

void reConstructBlocks(){
    //clear shard
    FOR(i,0,block_num){
        blocks[i].clear();
    }
    FOR(i,0,nodes){
        blocks[prevShard[i]].push_back((int)i);
    }
}

//print weights J, JL and h
void printWeights(){
    cout<<"\nPrinting calculated weights..."<<endl;
    cout<<"The value of J is: "<<J<<endl;
    cout<<"The value of JL is: "<<JL<<endl;
    cout<<"The values of h are: ";
    FOR(i,0,block_num){
        cout<<h[i]<<" ";
    }
    cout<<endl;
}

//prior values for a+0, b+0, a-0, b-0, vec{n0}
double ap0=2;
double bp0=1;
double am0=1;
double bm0=2;

double* vecN;

//start of main program
int main(int argc, const char * argv[]){
    
    //get stdin from shell script
    fileName=argv[1];
    block_num=atoi(argv[2]);
    iterations=atoi(argv[3]);
    
    if(argc==5) verbose=argv[4];
    
    //initialize n using block numbers
    vecN = new double[block_num];
    FOR(i,0,block_num){
        vecN[i]=1.0;
    }
   
    //Reading from graphs
    inFile.open(fileName,ios::in);

    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }

    //read number of nodes and edges
    inFile>>nodes>>edges;

    //produce adjList && adjMatrix
    createADJ();
    //convertADJtoADJMatrix();

    //initialize prevShard for O(1) block assignment query
    prevShard = new int[nodes];

    //Bayesian Approach to identify block structure in the network
    //Variational method for approximate inference
    //Variant of BLP which takes a discounted vote over neighbors membership
    //1.Randomly initialize the block assignments
    //pi_weights = new double[block_num];
    blocks = new vector<int>[block_num];
    randomAssignment();
    double locality=printLocatlityFraction();
    printf("After random assignments, the locality is: %lf\n\n", locality);
    
    //repeat discounted vote process until convergence in FA[q] variational free energy
    while(iterations--){
        printf("\nStarting clustering iteration.....\n");
        //count the sizes of each block into blockSize vector
        countBlockSize();
        printBlockSize();
        //print_blocks_assignments();
        
        //count the number of edges from the observed network
        int mpp = countEdgesWithinComm();
        int mpm = countNonEdgesWithinComm(mpp);
        int mmp = countEdgesBetweenComm(mpp);
        int mmm = countNonEdgesBetweenComm(mmp);
        
        //calculating discounted votes
        J = DIGAMMA(mpp+ap0) - DIGAMMA(mpm+bp0) - DIGAMMA(mpm+am0) + DIGAMMA(mmm+bm0);
        JL = DIGAMMA(mmm+bm0) - DIGAMMA(mmp+am0+mmm+bm0) - DIGAMMA(mpm+bp0) + DIGAMMA(mpp+ap0+mpm+bp0);
        h = new double[block_num];
        
        for(int i=0;i<block_num;i++){
            int ak_sum=0;
            for(int j=0;j<block_num;j++) ak_sum+=vecN[j];
            h[i] = DIGAMMA(blocks[i].size()+vecN[i]) - DIGAMMA(ak_sum);
        }
        
        if(verbose=="v"){
            //Debugging weights
            printWeights();
        }
        else{
            cout<<"\nWeights calculations are done."<<endl;
        }
        
        //sub in formula for discounted vote
        int move_cnt = 0;
        for(int i=0;i<nodes;i++){
            int new_assignment = findBestAssignmentK((int)i,J,JL,h);
            if(prevShard[i]!=new_assignment) move_cnt++;
            prevShard[i] = new_assignment;
        }
    //    //Map blocks to shards
    //    //Or collapse nodes to use lpsolve
    //
    //    //    printShard();
    //    //output locality info to shell
        printf("There are %d nodes moved in this iteration\n", move_cnt);
        locality=printLocatlityFraction();
        cout<<"After posterior assignments, the locality is: "<<locality<<endl;
        reConstructBlocks();
    }
//
//    //write data to file for graph plotting
//    FILE* outFile=fopen("graph_plotting_data.txt","w");
//    fprintf(outFile,"node locality\n");
//    fprintf(outFile,"%d %lf\n",nodes,locality);
//    fclose(outFile);
//
//    //fprint to files
//    outFile=fopen("sharding_result.bin", "wb");
//
//    //write block of data to stream
//    //    fwrite(prevShard, sizeof(int), nodes, outFile);
//    //    fwrite(shard, sizeof(vector<int>), partitions, outFile);
//    for(int i=0;i<partitions;i++){
//        fprintf(outFile,"%d\n",int(shard[i].size()));
//        for(int j=0;j<shard[i].size();j++){
//            fprintf(outFile,"%d ", shard[i][j]);
//        }
//        fprintf(outFile,"\n");
//    }
//
//    for(int i=0;i<nodes;i++){
//        fprintf(outFile,"%d ", prevShard[i]);
//    }
//
    //delete dynamic allocation
    delete [] blocks;
    delete [] prevShard;
    delete [] adjList;
    delete [] h;
    delete [] vecN;
    
    /*
    FOR(i,0,nodes){
        delete [] adjMatrix[i];
    }

    delete [] adjMatrix;
     */
    
    //remove dangling pointers
    blocks=nullptr;
    adjList=nullptr;
    prevShard=nullptr;
    vecN=nullptr;
    h=nullptr;
    //adjMatrix=nullptr;
//    fclose(outFile);
//
    return 0;
}
