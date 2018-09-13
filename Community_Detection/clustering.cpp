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

using namespace std;

#define FOR(i,a,b) for(size_t i=a;i<b;i++)

//global variables here
vector<int>* shard;
int* prevShard;
vector<int>* adjList;
fstream inFile;
string fileName;
int nodes;
int edges;

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

void createADJ(){
    int from,to;
    while(inFile>>from>>to){
        adjList[from].push_back(to);
        // comment out the following line if the graph is directed
        adjList[to].push_back(from);
    }
}

//start of main program
int main(int argc, const char * argv[]){
    
    //get stdin from shell script
    fileName=argv[1];
    
    inFile.open(fileName,ios::in);
    
    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    
    //read number of nodes and edges
    inFile>>nodes>>edges;
    
    //create data structures with variable size on the heap
    adjList=new vector<int>[nodes];
    
    //produce adjList
    createADJ();
    //Bayesian Approach to identify block structure in the network
    //Variational method for approximate inference
    //Variant of BLP which takes a discounted vote over neighbors membership
    //1.Randomly initialize the block assignments
    
    //Map blocks to shards
    //Or collapse nodes to use lpsolve
    
    //    printShard();
    //output locality info to shell
    double locality=printLocatlityFraction();
    
    //write data to file for graph plotting
    FILE* outFile=fopen("graph_plotting_data.txt","w");
    fprintf(outFile,"node locality\n");
    fprintf(outFile,"%d %lf\n",nodes,locality);
    fclose(outFile);
    
    //fprint to files
    outFile=fopen("sharding_result.bin", "wb");
    
    //write block of data to stream
    //    fwrite(prevShard, sizeof(int), nodes, outFile);
    //    fwrite(shard, sizeof(vector<int>), partitions, outFile);
    for(int i=0;i<partitions;i++){
        fprintf(outFile,"%d\n",int(shard[i].size()));
        for(int j=0;j<shard[i].size();j++){
            fprintf(outFile,"%d ", shard[i][j]);
        }
        fprintf(outFile,"\n");
    }
    
    for(int i=0;i<nodes;i++){
        fprintf(outFile,"%d ", prevShard[i]);
    }
    
    //delete dynamic allocation
    delete [] shard;
    delete [] prevShard;
    delete [] adjList;
    
    //remove dangling pointers
    shard=nullptr;
    adjList=nullptr;
    prevShard=nullptr;
    
    fclose(outFile);
    
    return 0;
}
