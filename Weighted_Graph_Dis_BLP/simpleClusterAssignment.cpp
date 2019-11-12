//  simpleClusterAssignment.cpp
//  non-greedy assignment to remove distributed computing bottleneck
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
#include <boost/log/trivial.hpp> // logging

using namespace std;

#define FOR(i,a,b) for(size_t i=a;i<b;i++)

//global variables here
vector<int>* shard;
int* prevShard;
int* nodesTranslation;
vector<int>* adjList;
vector<int>* blocks;
vector<pair<int,int>> block_sizes;
unordered_map<int,int>* weighted_adjList;
fstream inFile;
string fileName;
int partitions;
int block_num;
int nodes;
int edges;
int* mass;
int* nodeBlockMap;
string verbose;

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
    if(verbose=="v") BOOST_LOG_TRIVIAL(debug)<<"counting edges between partition "<<y<<" and block "<<z<<endl;
    int count = 0;
    //each of the nodes in block[z] check it adjMatrix with each node in partition y whether edge exists
    FOR(i,0,blocks[z].size()){
        int node = blocks[z][i];
        FOR(j,0,shard[y].size()){
            if(check_connection(shard[y][j],node)) count++;
        }
    }
    if(verbose=="v") BOOST_LOG_TRIVIAL(debug)<<"the count is: "<<count<<endl;
    return count;
}

int chooseLargestUnallocatedBlock(int* allocation){
    //then chose the largest unassigned_block
    FOR(i,0,block_num){
        int next_largest_block=block_sizes[i].second;
        if(allocation[next_largest_block]==0) {
            if(verbose=="v") BOOST_LOG_TRIVIAL(debug)<<"the largest unallocated block is: "<<next_largest_block<<endl;
            int block_to_assign=next_largest_block;
            return block_to_assign;
        }
    }
    return -1;
}

int nodeFindBlock(int pivot_id){
    return nodeBlockMap[pivot_id];
}

void buildNodeBlockMap(){
    FOR(i,0,block_num){
        FOR(j,0,blocks[i].size()){
            nodeBlockMap[blocks[i][j]]=i;
        }
    }
}

//serpentine assignment to assign blocks to partitions
//when reaching boundary then check and break sizes
void serpentineAssignment(){
    //sort blocks by decreasing size |Bk|
    //put into pairs of (size_k, k)
    FOR(i,0,block_num){
        block_sizes.emplace_back((int)blocks[i].size(),i);
    }
    
    sort(block_sizes.begin(), block_sizes.end(), Greater());
    
    //test whether they are sorted and check sum
    int sum=0;
    FOR(i,0,block_num){
        if(verbose=="v") BOOST_LOG_TRIVIAL(debug)<<block_sizes[i].first<<" "<<block_sizes[i].second<<endl;
        sum+=block_sizes[i].first;
    }
    if(verbose=="v") BOOST_LOG_TRIVIAL(debug)<<"sum of the nodes are: "<<sum<<endl<<endl;
    
    //caculate size celling for all partitions and intialize them
    int size_limit=nodes/partitions+1;
    int* space_left = new int[partitions];
    
    FOR(i,0,partitions){
        space_left[i]=size_limit;
    }
    
    //using serpentine method to assign blocks to partitions, from largest to smallest
    int shard_id=0; bool increasing=true;
    
    //shard[partitions] vector is the partitions container
    FOR(i,0,block_num){
        if(verbose=="v"){
            BOOST_LOG_TRIVIAL(debug)<<"In allocation loop"<<endl;
            BOOST_LOG_TRIVIAL(debug)<<"current assigning block is: "<<i<<endl;
        }
        
        if(verbose=="v") BOOST_LOG_TRIVIAL(debug)<<"The size of block is: "<<block_sizes[i]<<endl;
        
        //assign first then decide increase or decrease
        int ranked_block_size = block_sizes[i].first;
        int block_id = block_sizes[i].second;
        FOR(j,0,ranked_block_size){
            int node_id = blocks[block_id][j];
            //two structures need to be updated, one is shard container, the other is node-shard mapping lookup
            shard[shard_id].push_back(node_id);
            prevShard[node_id]=shard_id;
        }
        //serpentine increase id to next shard
        if(increasing && shard_id<=partitions-1){
            shard_id++;
            if(shard_id==partitions){
                shard_id--;
                increasing=false;
            }
            
        }
        //serpentine decrease id to previous shard
        else if(!increasing && shard_id>=0){
            shard_id--;
            if(shard_id==-1){
                shard_id++;
                increasing=true;
            }
        }
        //this case should never happen
        else {
            BOOST_LOG_TRIVIAL(error)<<"Error with shard id: "<<shard_id<<endl;
        }
    }
    
    //check sizes
    BOOST_LOG_TRIVIAL(debug)<<endl<<"Sizes of partitions after serpentine assignments: "<<endl;
    FOR(i,0,partitions) BOOST_LOG_TRIVIAL(debug)<<shard[i].size()<<" "<<endl;

}

void buildPrevShard(){
    FOR(i,0,partitions){
        FOR(j,0,shard[i].size()){
            prevShard[shard[i][j]]=(int)i;
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
    
    string clusterFile = fileName.substr(0,fileName.find(".")) + "_clusters.txt";
    inFile.open(clusterFile,ios::in);
    
    if(!inFile){
        BOOST_LOG_TRIVIAL(debug)<<"Error occurs while opening the file"<<endl;
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

//using translation table and eligible communities assignments(blocks) to form node weights(mass) and edge weights(weighted_adjList) to combine communities into a single node
void combineCommunities(){
    //initialize mass to 1
    FOR(i,0,nodes){
        mass[i]=1;
    }
    //transform mass according to comm
    FOR(j,0,block_num){
        //pivot node mass increases to community size if the comm size is not 0
        if(blocks[j].size()>0) mass[blocks[j][0]]=blocks[j].size();
        //reduce the mass of subordinate nodes
        FOR(k,1,blocks[j].size()){
            mass[blocks[j][k]]=0;
        }
    }
    //form weighted adjList
    //pivot node means the first node of a community or an individual node
    FOR(y,0,nodes){
        FOR(z,0,adjList[y].size()){
            int pivot_x=nodesTranslation[y];
            int pivot_y=nodesTranslation[adjList[y][z]];
            //if they do not have the same pivot node, means it is an external edges
            if(pivot_x!=pivot_y){
                //check whether this edge is already added with weight before
                //if we already pushed this edge, add 1 to its weight
                if(weighted_adjList[pivot_x].find(pivot_y)!=weighted_adjList[pivot_x].end()){
                    weighted_adjList[pivot_x][pivot_y]++;
                }
                //else add this entry to the hash map
                else {
                    weighted_adjList[pivot_x][pivot_y]=1;
                }
            }
        }
    }
}

//start of main program
int main(int argc, const char * argv[]){
    
    //get stdin from shell script
    fileName=argv[1];
    partitions=atoi(argv[2]);
    
    if(argc==4) verbose=argv[3];
    
    inFile.open(fileName,ios::in);
    
    if(!inFile){
        BOOST_LOG_TRIVIAL(debug)<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    
    //read number of nodes and edges
    inFile>>nodes>>edges;
    
    //create data structures with variable size on the heap
    shard=new vector<int>[partitions];
    prevShard=new int[nodes];
    adjList=new vector<int>[nodes];
    nodesTranslation=new int[nodes];
    mass=new int[nodes];
    weighted_adjList=new unordered_map<int,int>[nodes];
    nodeBlockMap=new int[nodes];
    
    //produce adjList
    createADJ();
    loadTranslationAndBlock();
    combineCommunities();
    buildNodeBlockMap();
    
    //random sharding
    greedyAssignment();
    
    BOOST_LOG_TRIVIAL(debug)<<"\nFinished simple cluster assignment"<<endl;
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
