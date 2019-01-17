//  greedyAssignment.cpp
//  Boost
//
//  Created by Steve DengZishi on 1/13/19.
//  Copyright © 2019 Steve DengZishi. All rights reserved.
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
double* edgeCountAvg;//edgeCount[block_num] represent average edges between current partition and block i
vector<int>* adjList;
vector<int>* blocks;
vector<pair<int,int>> block_sizes;
fstream inFile;
string fileName;
int partitions;
int block_num;
int nodes;
int edges;

struct Greater
{
    template<class T>
    bool operator()(T const &a, T const &b) const { return a > b; }
};

//functions here
//replace adjMatrix using adjList in checking edge connection
int check_connection(int i, int j){
    FOR(k,0,adjList[i].size()){
        if(adjList[i][k] == j) return 1;
    }
    return 0;
}

int countEdgesBetweenPartitionAndBlock(int y, int z){
    int count = 0;
    //each of the nodes in block[z] check it adjMatrix with each node in partition y whether edge exists
    FOR(i,0,blocks[z].size()){
        int node = blocks[z][i];
        FOR(j,0,shard[y].size()){
            if(check_connection(shard[y][j],node)) count++;
        }
    }
    return count;
}

int chooseLargestUnallocatedBlock(int* allocation){
    //then chose the largest unassigned_block
    FOR(i,0,block_num){
        int next_largest_block=block_sizes[i].second;
        if(allocation[next_largest_block]==0) {
            cerr<<"the largest unallocated block is: "<<next_largest_block<<endl;
            int block_to_assign=next_largest_block;
            return block_to_assign;
        }
    }
    return -1;
}
//greedy assignment to assign blocks to partitions
void greedyAssignment(){
    //sort blocks by decreasing size |Bk|
    //put into pairs of (size_k, k)
    FOR(i,0,block_num){
        block_sizes.emplace_back((int)blocks[i].size(),i);
    }
    
    sort(block_sizes.begin(), block_sizes.end(), Greater());
    
    //test whether they are sorted and check sum
    int sum=0;
    FOR(i,0,block_num){
        cerr<<block_sizes[i].first<<" "<<block_sizes[i].second<<endl;
        sum+=block_sizes[i].first;
    }
    cerr<<"sum of the nodes are: "<<sum<<endl<<endl;
    
    //while there is unallocated blocks
    bool unallocated=true;
    int* allocation=new int[block_num];
    FOR(i,0,block_num) allocation[i]=0;
    int current_partition=0;
    int size_limit=nodes/partitions+1;
    int current_space=size_limit;
    edgeCountAvg=new double[block_num];
    
    while(unallocated && current_partition<partitions){
        cerr<<"In allocation loop"<<endl;
        cerr<<"current partition is: "<<current_partition<<endl;
        int block_to_assign=-1;
        //start with the first shard, if it is empty
        if(shard[current_partition].size()==0){
            cerr<<"current shard is empty, select largest unallocated block"<<endl;
            //then chose the largest unassigned_block
            block_to_assign=chooseLargestUnallocatedBlock(allocation);
        }
        
        //else select the block with largest average in-degree & unallocated
        else{
            cerr<<"select largest average in-degree block"<<endl;
            //need to clear previous edge counts as after new assignment it is changing
            FOR(i,0,block_num){
                edgeCountAvg[i]=0.0;
            }
            FOR(i,0,block_num){
                //if still unallocated
                if(allocation[i]==0){
                    edgeCountAvg[i]=(double)countEdgesBetweenPartitionAndBlock(current_partition,i)/(double)blocks[i].size();
                }
            }
            int max=0;
            FOR(i,0,block_num){
                if(edgeCountAvg[i]!=0.0 && edgeCountAvg[max]<edgeCountAvg[i]) max=i;
            }
            if(edgeCountAvg[max]!=0.0) block_to_assign=max;
            //if there is no in-degree edges
            if(block_to_assign==-1) block_to_assign=chooseLargestUnallocatedBlock(allocation);
        }
        
        //all blocks have been allocated
        if(block_to_assign==-1){
            unallocated=false;
            break;
        }
        //start assigning process
        //first need to do boundary check
        //if block size is greater than remaining space
        //overflowing to the next shard
        if(blocks[block_to_assign].size()>current_space){
            int current_shard=current_space;
            int next_shard=blocks[block_to_assign].size()-current_space;
            //moving nodes to this shard
            FOR(i,0,current_shard){
                int moving_node=blocks[block_to_assign][i];
                prevShard[moving_node]=current_partition;
                shard[current_partition].push_back(moving_node);
            }
            //overflowing to the next shard
            current_space=size_limit-next_shard;
            current_partition++;
            FOR(i,current_shard,blocks[block_to_assign].size()){
                int moving_node=blocks[block_to_assign][i];
                prevShard[moving_node]=current_partition;
                shard[current_partition].push_back(moving_node);
            }
        }
        //else still in this shard
        else{
            int current_shard=blocks[block_to_assign].size();
            current_space-=current_shard;
            //moving nodes
            FOR(i,0,blocks[block_to_assign].size()){
                int moving_node=blocks[block_to_assign][i];
                prevShard[moving_node]=current_partition;
                shard[current_partition].push_back(moving_node);
            }
        }
        allocation[block_to_assign]=1;
        cerr<<"block to allocate is: "<<block_to_assign<<endl;
        cerr<<"space left in current shard is: "<<current_space<<endl;
    }
    
    //check sizes
    cerr<<endl<<"Sizes of partitions after greedy assignments: "<<endl;
    FOR(i,0,partitions) cerr<<shard[i].size()<<" "<<endl;
    
    //clear dynamic allocated memory
    delete [] edgeCountAvg;
    edgeCountAvg=nullptr;
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
    inFile.close();
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

void loadTranslationAndBlock(){
    //init array
    nodesTranslation=new int[nodes];
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
    
    //save blocks to be used in combineNodes
    blocks=new vector<int>[block_num];
    
    //each line start with a size, and size number of nodes with the first as the pivot
    FOR(i,0,block_num){
        int size;
        int pivot;
        inFile>>size;
        if(size){
            inFile>>pivot; blocks[i].push_back(pivot);
            FOR(j,1,size){
                int sub_node; inFile>>sub_node;
                nodesTranslation[sub_node]=pivot;
                blocks[i].push_back(sub_node);
            }
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
    loadTranslationAndBlock();
    //random sharding
    greedyAssignment();
    cerr<<"\nFinished greedy assignment"<<endl;
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
