//
//  applyMove.cpp
//  Boost
//
//  Created by Steve DengZishi on 7/17/17.
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
#define ALL(a) a.begin(),a.end()
#define CINLINE(a) getline(cin,a)
#define FILL(a,b) memset(a, b , sizeof(a)) //fill array a with all bs
#define INIT(a) FILL(a,0) //initialize array a with all 0s
//#define lp_ingredient_5th

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

int* prevShard;
vector<int>* adjList;
vector<PII>** sortedCountIJ;
vector<int>* shard;
string fileName;
string x_file;
fstream inFile;
int partitions;
int nodes;
int edges;
int move_count;

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

// cut the ListSize after getting X(ij) values from the linear functions
void cutList(){
    FOR(i,0,partitions){
        FOR(j,0,partitions){
            if(i!=j){
                int fromTo; char x;
                inFile>>x>>fromTo;
                int size; inFile>>size;
                if(size<0) size=0;
                //cerr<<"size ("<<i<<","<<j<<") is: "<<size<<endl;
                //cerr<<"vector size: "<<sortedCountIJ[i][j].size()<<endl;
                //cerr<<"resize is: "<<size<<endl;
                sortedCountIJ[i][j].resize(size);
            }
        }
    }
}

//directly using cutted sortedCountIJ to apply movement and count movement at the same time
void applyShift(){
    move_count=0;
    FOR(i,0,partitions){
        FOR(j,0,partitions){
            if(i!=j){
                //cerr<<i<<j<<" the number of nodes movement are: "<<sortedCountIJ[i][j].size()<<endl;
                FOR(k,0,sortedCountIJ[i][j].size()){
                    //update the location of the previous node for next iteration
                    //cerr<<prevShard[sortedCountIJ[i][j][k].second]<<" becomes "<<j<<endl;
                    prevShard[sortedCountIJ[i][j][k].second]=(int)j;
                    move_count++;
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

void loadShard(){
    //random sharding according using a integer hash then mod 8 to distribute to shards
    FILE* inFile=fopen("sharding_result.bin", "rb");
    
    //    fseek(inFile,0,SEEK_SET);
    //    fread(shard, sizeof(vector<int>), partitions, inFile);
    //    fread(prevShard, sizeof(int), nodes, inFile);
    
    // use fscanf instead of fread
    //shard
    for(int i=0;i<partitions;i++){
        int size;
        fscanf(inFile,"%d",&size);
        for(int j=0;j<size;j++){
            int data;
            fscanf(inFile,"%d",&data);
            shard[i].push_back(data);
        }
    }
    
    //prevShard
    for(int i=0;i<nodes;i++){
        fscanf(inFile,"%d",&prevShard[i]);
    }
    
    //sortedCountIJ
    for(int i=0;i<partitions;i++){
        for(int j=0;j<partitions;j++){
            int size;
            fscanf(inFile,"%d",&size);
            for(int k=0;k<size;k++){
                int first,second;
                fscanf(inFile,"%d %d",&first,&second);
                sortedCountIJ[i][j].emplace_back(first,second);
            }
        }
    }
    
    fclose(inFile);
}

void printTotalMovement(){
    printf("\n%d nodes out of %d nodes made their movement in this iteration\n",move_count,nodes);
}



int main(int argc, const char * argv[]){
    cerr<<"In applyMove"<<endl;
    cerr<<"------------------------------------"<<endl;
    //get stdin from shell script
    fileName=argv[1];
    partitions=atoi(argv[2]);
    x_file=argv[3];
    
    inFile.open(fileName,ios::in);
    
    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    
    //read number of nodes and edges
    inFile>>nodes>>edges;
    
    //allocate memory on the heap
    shard=new vector<int>[partitions];
    prevShard=new int[nodes];
    adjList=new vector<int>[nodes];
    
    sortedCountIJ=new vector<PII>* [partitions];
    for(int i=0;i<partitions;i++){
        sortedCountIJ[i]=new vector<PII> [partitions];
    }
    //create adjacency List
    createADJ();
    inFile.close();
    
    //load sharding and previously calculated results
    //load previous shard[partitions], prevShard[nodes] & loadSortecCountIJ[partitions][partitions]
    loadShard();
    
    //close file and open x_result file
    inFile.open(x_file,ios::in);
    
    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }

    //Three steps to move nodes after the linear program returns constraints X(ij), input values with files injection in cutList()
    cutList();
    applyShift();
    reConstructShard();

    //print movement info
    printTotalMovement();
    //output locality info to shell
    double locality=printLocatlityFraction();
    
    //check sizes
    cerr<<endl<<"Sizes of partitions after linear programming: "<<endl;
    int sum=0;
    FOR(i,0,partitions) {
        cerr<<shard[i].size()<<" "<<endl;
        sum+=shard[i].size();
    }
    cerr<<"the sum of the sizes is: "<<sum<<endl;
    
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
    
    //clear dynamic memory on the heap
    delete [] shard;
    delete [] adjList;
    delete [] prevShard;
    
    for(int i=0;i<partitions;i++){
        delete [] sortedCountIJ[i];
    }
    delete [] sortedCountIJ;
    
    //remove dangling pointers
    shard=nullptr;
    adjList=nullptr;
    prevShard=nullptr;
    sortedCountIJ=nullptr;
    
    cerr<<"------------------------------------"<<endl;
}
