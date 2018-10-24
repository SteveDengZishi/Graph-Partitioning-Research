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

using namespace std;

#define FOR(i,a,b) for(size_t i=a;i<b;i++)

//global variables here
vector<int>* blocks;
int* prevShard;
vector<int>* adjList;
int** adjMatrix;
double* pi_weights;
fstream inFile;
string fileName;
int block_num;
int nodes;
int edges;
double J;
double J_1;
double* h;

//functions here
unsigned int int_hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
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
    printf("Before iterations, there are %d local edges out of total %d edges, fraction of local edges is: %lf\n\n",localEdge,totalEdge,fraction);
    return fraction;
}

//build adjacency list from streaming edges
void createADJ(){
    int from,to;
    while(inFile>>from>>to){
        adjList[from].push_back(to);
        // comment out the following line if the graph is directed
        adjList[to].push_back(from);
    }
}

//convert adjacency list to adjacency matrix
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

//m++, c+
int countEdgesWithinComm(){
    int count = 0;
    return count;
}

//m+-, c-
int countNonEdgesWithinComm(){
    int count = 0;
    return count;
}

//m-+, d+
int countEdgesBetweenComm(){
    int count = 0;
    return count;
}

//m--, d-.

int countNonEdgesBetweenComm(){
    int count = 0;
    return count;
}


void generate_pi_weights(){
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
}

void print_blocks_assignments(){
    for(int i=0;i<block_num;i++){
        cout<<"In block "<<i<<endl;
        for(int j=0;j<blocks[i].size();j++){
            cout<<blocks[i][j]<<" ";
        }
        cout<<endl;
    }
}

//start of main program
int main(int argc, const char * argv[]){
    
    //get stdin from shell script
    fileName=argv[1];
    block_num=atoi(argv[2]);
    
    //temp before using actual file
    nodes=100;
    
//    inFile.open(fileName,ios::in);
//
//    if(!inFile){
//        cerr<<"Error occurs while opening the file"<<endl;
//        exit(1);
//    }
//
//    //read number of nodes and edges
//    inFile>>nodes>>edges;
//
//    //create data structures with variable size on the heap
//    adjList=new vector<int>[nodes];
//
//    //produce adjList
//    createADJ();
    
    //Bayesian Approach to identify block structure in the network
    //Variational method for approximate inference
    //Variant of BLP which takes a discounted vote over neighbors membership
    //1.Randomly initialize the block assignments
    pi_weights = new double[block_num];
    blocks = new vector<int>[block_num];
    
    //generate pi vector with respective weights following Dirichlet distribution
    generate_pi_weights();
    print_pi_weights();
    
    //randomly assign all nodes to blocks according to their pi biases
    for(int i=0;i<nodes;i++){
        //compute block assignment according to bias and store block assignment in blocks vectors
        int block_assignment = get_block_assignment();
        blocks[block_assignment].push_back(i);
    }
    
    print_blocks_assignments();
    
    //repeat discounted vote process until convergence in FA[q] variational free energy
    //while(convergenence condition reached) do
    J = //expected formula
    J_1 = //expected formula
    h = new double[block_num];
    for(int i=0;i<block_num;i++){
        h[i] = //expected formula
    }
    
    //sub in formula for discounted vote
    for(int i=0;i<nodes;i++){
        
    }
//    //Map blocks to shards
//    //Or collapse nodes to use lpsolve
//
//    //    printShard();
//    //output locality info to shell
//    double locality=printLocatlityFraction();
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
//    //delete dynamic allocation
//    delete [] shard;
//    delete [] prevShard;
//    delete [] adjList;
//
//    //remove dangling pointers
//    shard=nullptr;
//    adjList=nullptr;
//    prevShard=nullptr;
//
//    fclose(outFile);
//
    return 0;
}
