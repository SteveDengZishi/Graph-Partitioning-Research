//
//  pairwise_partition_swap.cpp
//  At final convergence, there can be chances where many nodes want to move but there is no space available on target partition using linear programming
//  We can utilize a pairwise partition swaps to maximize gains while kick out some nodes that have small negative gain to make room
//
//  Boost
//
//  Created by Steve DengZishi on 11/25/19.
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
#include <fstream> // To input and output to files
#include <limits>


//macro here
#define FOR(i,a,b) for(size_t i=a;i<b;i++)
#define DE(x) cout << #x << ":" << x << endl
#define DEIF cout<<"DEBUG: in if"<<endl
#define DEELSE cout<<"DEBUG: in else"<<endl
#define DEIFIF cout<<"DEBUG: in if if"<<endl
#define DEIFELSE cout<<"DEBUG: in if else"<<endl
#define DEELSEIF cout<<"DEBUG: in else if"<<endl
#define DEELSEELSE cout<<"DEBUG: in else else"<<endl
#define BEFORE cout<<"before"<<endl;
#define AFTER cout<<"after"<<endl;
#define ALL(a) a.begin(),a.end()
#define CINLINE(a) getline(cin,a)
#define FILL(a,b) memset(a, b , sizeof(a)) //fill array a with all bs
#define INIT(a) FILL(a,0) //initialize array a with all 0s

//name space here
using namespace std;

//typedef here
typedef vector<int> VI;
typedef vector<pair<int,pair<int,int>>> Edges;
typedef pair<int,int> PII;

struct Greater
{
    template<class T>
    bool operator()(T const &a, T const &b) const { return a > b; }
};

//functions, global variables, comparators & Non-STL Data Structures definition here


//modified to pointers to allow dynamically allocate on heap
vector<int>* shard;
vector<int>* adjList;
unordered_map<int,int>* weighted_adjList;
vector<pair<int,int>>** sortedCountIJ;//in each ij pairs, stores sorted (gains,node)
int** neighbors; //[nodeID][shard] gives number of neighbors
vector<int>* blocks;
int* score;
vector<int> Pcount;
int partitions;
int nodes;
int edges;
int block_num;
fstream inFile;
string fileName;
// Stores the first choices of all nodes after each iteration
int* prevShard;
int move_count;

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

void printSortedCount(int i, int j){
    printf("The increase in colocation from shard %d to %d is: ",i,j);
    for(int k=0;k<sortedCountIJ[i][j].size();k++){
        printf(" (%d,%d) ",sortedCountIJ[i][j][k].first,sortedCountIJ[i][j][k].second);
    }
    cout<<endl<<endl;
}

/*
 //producing increase in locality count and sort the result
 void fSort(int i, int j){
 for(int k=0;k<shard[i].size();k++){
 //Calculate the increase in count (INC)
 int INC=neighbors[shard[i][k]][j]-neighbors[shard[i][k]][i];
 PII p(INC,shard[i][k]);
 sortedCountIJ[i][j].push_back(p);
 }
 sort(ALL(sortedCountIJ[i][j]),Greater());
 //    printSortedCount(i,j);
 }
 */

//directly produce SortedCount to save time mapping and sorting
void produceSortedCountIJ(){
    for(int i=0;i<nodes;i++){
        //the position (shardID) of the current node
        int from=prevShard[i];
        
        //directly filter out the top gain movement, by default was itself at current position
        //pairwise swap allows small negative gains to be kicked out
        int maxGain= -1 * numeric_limits<int>::max();
        int maxDest=from;
        
        for(int to=0;to<partitions;to++){
            //for any other shard other than the current node
            if(from!=to){
                //neighbors at other shard - neighbors at current shard
                int incr_cnt=neighbors[i][to]-neighbors[i][from];
                if(incr_cnt>maxGain){
                    maxGain=incr_cnt;
                    maxDest=to;
                }
            }
        }
        //check whether there is a effective move option, put directly into sortedcountIJ
        if (maxGain!=0 && maxDest!=from){
            sortedCountIJ[from][maxDest].emplace_back(maxGain,i);
        }
    }
}

//create original unweighted adjacency list from the input graph
void createADJ(){
    int from,to;
    while(inFile>>from>>to){
        adjList[from].push_back(to);
        // comment out the following line if the graph is directed
        adjList[to].push_back(from);
    }
}

void printADJ(){
    for(int i=0;i<nodes;i++){
        cout<<i<<": ";
        for(int j=0;j<adjList[i].size();j++){
            cout<<adjList[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}

void printShardSize(){
    FOR(i,0,partitions){
        cout<<shard[i].size()<<" "<<endl;
    }
    cout<<endl;
}

void loadShard(){
    //random sharding according using a integer hash then mod 8 to distribute to shards
    FILE* inFile=fopen("sharding_result.bin", "rb");
    
    //    fseek(inFile,0,SEEK_SET);
    //    fread(shard, sizeof(vector<int>), partitions, inFile);
    //    fread(prevShard, sizeof(int), nodes, inFile);
    
    // use fscanf instead of fread
    for(int i=0;i<partitions;i++){
        int size;
        fscanf(inFile,"%d",&size);
        for(int j=0;j<size;j++){
            int data;
            fscanf(inFile,"%d",&data);
            shard[i].push_back(data);
        }
    }
    for(int i=0;i<nodes;i++){
        fscanf(inFile,"%d",&prevShard[i]);
    }
    
    fclose(inFile);
}

void createNeighborList(){
    FOR(i,0,nodes){
        //reset the score array
        FOR(k,0,partitions) score[k]=0;
        //go through each neighbor of that node and add score count to the shard it is in
        FOR(j,0,adjList[i].size()){
            int where=prevShard[adjList[i][j]];
            score[where]++;
        }
        //assign tempt result to neighbor array
        FOR(z,0,partitions){
            neighbors[i][z]=score[z];
        }
    }
}

void printNeighborList(){
    FOR(i,0,nodes){
        printf("for node %d: ",(int)i);
        FOR(j,0,partitions){
            printf("%d ",neighbors[i][j]);
        }
        cout<<endl;
    }
}

void clearSortedCount(){
    FOR(i,0,partitions){
        FOR(j,0,partitions){
            sortedCountIJ[i][j].clear();
        }
    }
}

void printTotal(){
    int total=0;
    FOR(i,0,partitions){
        total+=shard[i].size();
    }
    cout<<"Total: "<<total<<endl;
}

void swapNodes(int i, int j, int count){
    cout<<"In Swaping Nodes between partitions "<<i<<" "<<j<<endl;
    FOR(z,0,count){
        //find each of the node swapping pairs
        int node_i_j = sortedCountIJ[i][j][z].second;
        int node_j_i = sortedCountIJ[j][i][z].second;
        //swapping the prevShard mapping of the corresponding nodes
        if(prevShard[node_i_j]==i && prevShard[node_j_i]==j){
            prevShard[node_i_j]=j;
            prevShard[node_j_i]=i;
        }
        else cerr<<"Critical Error: swapping partitions "<<i<<" "<<j<<" are in wrong state. This should never happen"<<endl;
    }
    cout<<count+1<<" number of node pairs swapped"<<endl;
}

//swapping nodes from pairwise partitions to kick out more nodes and make space for the swaps
void pairwiseSwap(){
    move_count=0;
    FOR(i,0,partitions){
        //only looping through when j > i for ij pair, to prevent duplicates
        FOR(j,i,partitions){
            if(i!=j){
                //send and rec vectors are all sorted with gains
                vector<PII> send_vec = sortedCountIJ[i][j];
                vector<PII> rec_vec = sortedCountIJ[j][i];
                int move_size = min(send_vec.size(), rec_vec.size());
                int total_gain=0;
                //within move size, only move top k pairs with pair net_gain > 0
                FOR(k,0,move_size){
                    int net_gain = send_vec[k].first + rec_vec[k].first;
                    //cout<<"net_gain for moving "<<k+1<<" pair is: "<<net_gain<<endl;
                    if(net_gain<=0){
                        //cout<<"stop swapping between partitions "<<i<<" "<<j<<endl;
                        //cout<<"number of nodes swapped is: "<<k+1<<endl;
                        //swap top k nodes among partition i & j
                        swapNodes(i,j,k);
                        cout<<"total gain is: "<<total_gain<<endl;
                        move_count+=k;
                        break;
                    }
                    else{
                        total_gain+=net_gain;
                    }
                }
            }
        }
    }
}

void reConstructShard(){
    //clear shard
    FOR(i,0,partitions){
        shard[i].clear();
    }
    FOR(i,0,nodes){
        shard[prevShard[i]].push_back((int)i);
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
    printf("There are %d local edges out of total %d edges, fraction of local edges is: %lf\n\n",localEdge,totalEdge,fraction);
    return fraction;
}

//start of main()
int main(int argc, const char * argv[]) {
    
    //get stdin from shell script
    fileName=argv[1];
    partitions=atoi(argv[2]);
    
    inFile.open(fileName,ios::in);
    
    if(!inFile){
        cout<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    
    //read number of nodes and edges
    inFile>>nodes>>edges;
    //cout<<nodes<<" "<<partitions<<endl;//(lp_ingredient)
    
    //allocate memory for adjList & sortedCountIJ on the heap
    shard=new vector<int>[partitions];
    prevShard=new int[nodes];
    adjList=new vector<int>[nodes];
    weighted_adjList=new unordered_map<int,int>[nodes];
    score=new int[partitions];
    
    //for showing movement of nodes between shards
    sortedCountIJ=new vector<pair<int,int>>*[partitions];
    for(int i=0;i<partitions;i++){
        sortedCountIJ[i]=new vector<pair<int,int>>[partitions];
    }
    
    //number of neighbors count for each node in each partition
    neighbors=new int*[nodes];
    for(int i=0;i<nodes;i++){
        neighbors[i]=new int[partitions];
    }
    
    //create adjacency list from edge list
    createADJ();
    //cout<<"after creating ADJ"<<endl;
    //    printADJ();
    inFile.close();
    
    //load previous shard[partitions] & prevShard[nodes]
    loadShard();
    //cout<<"after loading shard"<<endl;
    //    printShard();
    
    //create neighbor list
    //    printADJ();
    createNeighborList();
    //    printNeighborList();
    
    //calculate, sort and print the increase in colocation count for all nodes moving from i to j
    //in the form (INC(increase in colocation),nodeID)
    //directly produce SortedCountIJ by looping all nodes and selecting top gain movements
    
    produceSortedCountIJ();
    
    FOR(i,0,partitions){
        FOR(j,0,partitions){
            if(i!=j){
                //sortedIJ only left with 1 top gain moving option
                //the results are sorted by gain per node
                sort(ALL(sortedCountIJ[i][j]),Greater());
                //                printSortedCount(i,j);
            }
        }
    }
    
    pairwiseSwap();
    reConstructShard();
    double locality = printLocatlityFraction();
    
//    //check sizes
//    cout<<endl<<"Sizes of partitions after linear programming: "<<endl;
//    int sum=0;
//    FOR(i,0,partitions) {
//        cout<<shard[i].size()<<" "<<endl;
//        sum+=shard[i].size();
//    }
//    cout<<"the sum of the sizes is: "<<sum<<endl;
    
    //write data to file for graph plotting
    FILE* outFile=fopen("graph_plotting_data.txt","a");
    fprintf(outFile,"%d %lf\n",move_count,locality);
    fclose(outFile);
    
    //should reconstruct shard if we finished swapping
    //fprint structured data to files to be reload later in applyMove
    outFile=fopen("sharding_result.bin","wb");
    
    for(int i=0;i<partitions;i++){
        fprintf(outFile,"%d\n",int(shard[i].size()));
        for(int j=0;j<shard[i].size();j++){
            fprintf(outFile,"%d ", shard[i][j]);
        }
    }
    
    for(int i=0;i<nodes;i++){
        fprintf(outFile,"%d ", prevShard[i]);
    }
    
    for(int i=0;i<partitions;i++){
        for(int j=0;j<partitions;j++){
            fprintf(outFile, "%d\n", (int)sortedCountIJ[i][j].size());
            for(int k=0;k<sortedCountIJ[i][j].size();k++){
                fprintf(outFile, "%d %d ", sortedCountIJ[i][j][k].first, sortedCountIJ[i][j][k].second);
            }
        }
    }
    
    fclose(outFile);
    
    //free up allocated memory
    delete [] shard;
    delete [] adjList;
    delete [] weighted_adjList;
    delete [] prevShard;
    delete [] score;
    delete [] blocks;
    
    for(int i=0;i<nodes;i++){
        delete [] neighbors[i];
    }
    
    delete [] neighbors;
    
    for(int i=0;i<partitions;i++){
        delete [] sortedCountIJ[i];
    }
    delete [] sortedCountIJ;
    
    //remove dangling pointers
    shard=nullptr;
    adjList=nullptr;
    weighted_adjList=nullptr;
    prevShard=nullptr;
    sortedCountIJ=nullptr;
    score=nullptr;
    neighbors=nullptr;
    blocks=nullptr;
    return 0;
}
