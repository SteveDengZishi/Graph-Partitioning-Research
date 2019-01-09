//
//  RandomAssignment.cpp
//  Boost
//
//  Created by Steve DengZishi on 7/13/17.
//  Copyright © 2017 Steve DengZishi. All rights reserved.
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
int* nodesTranslation;
vector<int>* adjList;
fstream inFile;
string fileName;
int partitions;
int nodes;
int edges;

//functions here
void randomAssignment(){
    srand(time(NULL));
    //random sharding according using a integer hash then mod 8 to distribute to shards
    for(int i=0;i<nodes;i++){
        int pivot_id = nodesTranslation[i];
        //if this is a pivot node, give random assignment
        if(pivot_id==i){
            //        cout<<"i is: "<<i<<endl;
            int block_assignment = rand() % block_num;
            //        cout<<"shard ID is: "<<shardID<<endl;
            shard[block_assignment].push_back(i);
            //mark prevShard for the 1st random Sharding
            prevShard[i] = block_assignment;
        }
    }
    //else assign them to the same shard with their pivots
    //but we need to make sure pivot nodes get assigned before any other members so in a separate loop
    for(int i=0;i<nodes;i++){
        int pivot_id = nodesTranslation[i];
        //assign non-pivot nodes to the same shards as their pivot
        if(pivot_id!=i){
            prevShard[i]=prevShard[pivot_id];
            shard[prevShard[pivot_id]].push_back(i);
        }
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

void printShard(){
    for(int i=0;i<partitions;i++){
        printf("Shard %d:\n",i);
        for(int j=0;j<shard[i].size();j++){
            cout<<shard[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}

void loadTranslation(){
    //init array
    FOR(z,0,nodes){
        nodesTranslation[z]=z;
    }
    inFile.open("clusters.txt",ios::in);
    
    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    //how many number of lines(communities)
    inFile>>block_num;
    //each line start with a size, and size number of nodes with the first as the pivot
    FOR(i,0,block_num){
        int size;
        int pivot;
        inFile>>size;
        inFile>>pivot;
        FOR(j,1,size){
            int sub_node; inFile>>sub_node;
            nodesTranslation[sub_node]=pivot;
        }
    }
    inFile.close();
}

//start of main program
int main(int argc, const char * argv[]){
    
    //get stdin from shell script
    fileName=argv[1];
    partitions=atoi(argv[2]);
    
    inFile.open(fileName,ios::in);
    
    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    
    //read number of nodes and edges
    inFile>>nodes>>edges;
    
    //create data structures with variable size on the heap
    shard=new vector<int>[partitions];
    prevShard=new int[nodes];
    adjList=new vector<int>[nodes];
    nodesTranslation=new int[nodes];
    
    //produce adjList
    createADJ();
    loadTranslation();
    //random sharding
    randomAssignment();
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
    
    fclose(outFile);
    
    //delete dynamic allocation
    delete [] shard;
    delete [] prevShard;
    delete [] adjList;
    delete [] nodesTranslation;
    
    //remove dangling pointers
    shard=nullptr;
    adjList=nullptr;
    prevShard=nullptr;
    nodesTranslation=nullptr;
    
    return 0;
}
