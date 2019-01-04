//
//  probDisruptiveMove.cpp
//  Boost
//
//  Created by Steve DengZishi on 3/21/18.
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
#include <fstream> // To input and output to files


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
#define INF 2e9

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
vector<pair<double, int>>* lowestRatio;//local ratio(local degree/degree)
vector<int> pool;
int** neighbors;
int* outSize;//tracking the number of node out from the shards
int partitions;
int seed;
double alpha;
int nodes;
int edges;
fstream inFile;
string fileName;

unsigned int int_hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

//Ratio based assignment
void produceLowestRatio(){
    FOR(i,0,partitions){
        FOR(j,0,shard[i].size()){
            //adding pairs of ((localDeg/degree), nodeID)
            double ratio=(double)neighbors[shard[i][j]][i]/(double)adjList[shard[i][j]].size();
            pair<double,int> ratioPair(ratio,shard[i][j]);
            lowestRatio[i].push_back(ratioPair);
        }
    }
}

void produceRatioPool(){
    //alpha is taken from the 50/round number
    FOR(i,0,partitions){
        //initialize moveCount
        int moveCnt=0;
        //reseting seed for random number generation
        srand(seed);
        //srand((unsigned)time(NULL));
        FOR(j,0,lowestRatio[i].size()){
            //cout<<"the ratio is "<<lowestRatio[i][j].first<<endl;
            double moveProb=alpha*(1-lowestRatio[i][j].first);
            //cout<<"move probability is "<<moveProb<<endl;
            bool move=(rand()%100)<(moveProb*100);
            //cout<<"move decision is "<<move<<endl;
            if(move){
                moveCnt++;
                pool.push_back(lowestRatio[i][j].second);
            }
        }
        outSize[i]=moveCnt;
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

// Stores the first choices of all nodes after each iteration
//vector<PII>* vecMove; // move[nodeID] -> vectors of (gain,destination); //only sorted and leave other options for 2nd moves
int* prevShard;

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

void reConstructShard(){
    //clear shard
    FOR(i,0,partitions){
        shard[i].clear();
    }
    FOR(i,0,nodes){
        shard[prevShard[i]].push_back((int)i);
    }
}

void printTotal(){
    int total=0;
    FOR(i,0,partitions){
        total+=shard[i].size();
    }
    cout<<"Total: "<<total<<endl;
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

//New pool reassignment algorithm using outSize
int reAssignPool(){
    random_shuffle(pool.begin(),pool.end());
    int start=0;
    int end=0;
    int move_count=0;
    
    for(int i=0;i<partitions;i++){
        end=start+outSize[i];
        for(int j=start;j<end;j++){
            if(prevShard[pool[j]]!=i){
                move_count++;
                prevShard[pool[j]]=i;
            }
        }
        start=end;
    }
    return move_count;
}

//create NeighborList to be used in finding nodes with minimum neighborCount
void createNeighborList(){
    //create local temp array
    int score[partitions];
    
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

//start of main()
int main(int argc, const char * argv[]) {
    
    //get stdin from shell script
    fileName=argv[1];
    partitions=atoi(argv[2]);
    seed=atoi(argv[3]);
    alpha=atof(argv[4]);
    
    inFile.open(fileName,ios::in);
    
    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    
    //read number of nodes and edges
    inFile>>nodes>>edges;
    //    cout<<nodes<<" "<<partitions<<endl;//(lp_ingredient)
    
    //allocate memory for vecMove, adjList & sortedCountIJ on the heap
    shard=new vector<int>[partitions];
    prevShard=new int[nodes];
    adjList=new vector<int>[nodes];
    outSize=new int[partitions];
    lowestRatio=new vector<pair<double,int>>[partitions];
    
    //number of neighbors count for each node in each partition
    neighbors=new int*[nodes];
    for(int i=0;i<nodes;i++){
        neighbors[i]=new int[partitions];
    }
    
    //create adjacency list from edge list
    createADJ();
    //    printADJ();
    inFile.close();
    
    //load previous shard[partitions] & prevShard[nodes]
    loadShard();
    //    printShard();
    
    //find bottom 10% of nodes in each shard in terms of colocation count
    //and then produce a pool for re-assignment
    createNeighborList();
    
    //Local Ratio based re-assignment
    produceLowestRatio();
    produceRatioPool();
    
    int move_count=reAssignPool();
    
    //use a common pool to take out 25% of the nodes from each of the nodes from each shard
    //int move_count = randomShuffle();
    
    //reconstruct shard[partitions]
    reConstructShard();
    
    //print edge locality information
    printf("%d nodes out of %d nodes made their movement in this iteration\n",move_count,nodes);
    double locality=printLocatlityFraction();
    
    //write data to file for graph plotting
    FILE* outFile=fopen("graph_plotting_data.txt","a");
    fprintf(outFile,"%d %lf\n",move_count,locality);
    fclose(outFile);
    
    //fprintf shard[partitions] & prevShard[nodes] to be used in next iteration
    outFile=fopen("sharding_result.bin","wb");
    
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
    
    //free up allocated memory
    delete [] shard;
    delete [] adjList;
    delete [] prevShard;
    delete [] outSize;
    delete [] lowestRatio;
    
    for(int i=0;i<nodes;i++){
        delete [] neighbors[i];
    }
    delete [] neighbors;
    
    //remove dangling pointers
    shard=nullptr;
    adjList=nullptr;
    prevShard=nullptr;
    outSize=nullptr;
    lowestRatio=nullptr;
    neighbors=nullptr;
    
    return 0;
}
