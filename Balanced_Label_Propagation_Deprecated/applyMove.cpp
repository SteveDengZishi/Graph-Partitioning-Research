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
#define INF 2e9
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

vector<PII>* vecMove; // move[nodeID] -> vectors of (gain,destination); //only sorted and leave other options for 2nd moves
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
                sortedCountIJ[i][j].resize(size);
            }
        }
    }
}

void printVecMove(){
    FOR(i,0,nodes){
        printf("The movement options for node %d in the form (gains,destination) are: ",int(i));
        FOR(j,0,vecMove[i].size()){
            printf(" (%d,%d) ",vecMove[i][j].first,vecMove[i][j].second);
        }
        cout<<endl;
    }
    cout<<endl<<endl;
}

void mapToMove(){
    //clear the move vector
    FOR(i,0,nodes){
        vecMove[i].clear();
    }
    FOR(i,0,partitions){
        FOR(j,0,partitions){
            //sortedCount after cut
            FOR(k,0,sortedCountIJ[i][j].size()){
                vecMove[sortedCountIJ[i][j][k].second].emplace_back(sortedCountIJ[i][j][k].first,j);//(gain,destination)
            }
        }
    }
    //sort to select the top gain moving option
    FOR(i,0,nodes){
        sort(ALL(vecMove[i]),Greater());
    }
}



//once each iteration
void applyShift(vector<PII>* m){ //m[nodeID] -> vectors of (gain,destination)
    //find the node & remove them from shard[harsh_int(nodeID)] & add to new destination shard
    FOR(i,0,nodes){
        if(m[i].size()!=0){
//            //using erase-remove idiom (too inefficient, reconstruct shard[nodes] instead)
//            shard[prevShard[i]].erase(remove(shard[prevShard[i]].begin(),shard[prevShard[i]].end(),i),shard[prevShard[i]].end());
//            //m[i][0] is each of the top gain movement option
//            shard[m[i][0].second].push_back(int(i));
            
            //update the location of the previous node for next iteration
            prevShard[i]=m[i][0].second;
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

int printTotalMovement(){
    int cnt=0;
    for(int i=0;i<nodes;i++){
        if(vecMove[i].size()==1) cnt++;
    }
    printf("%d nodes out of %d nodes made their movement in this iteration\n",cnt,nodes);
    return cnt;
}



int main(){
    
    //get stdin from shell script
    cin>>fileName;
    cin>>partitions;
    cin>>x_file;
    
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
    vecMove=new vector<PII>[nodes];//do not have to load as it will be overidden 
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
    mapToMove();
    //    printVecMove(); //Debugging line
    
    applyShift(vecMove);
    reConstructShard();
    int move_count=printTotalMovement();
    //output locality info to shell
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
    
    //clear dynamic memory on the heap
    delete [] shard;
    delete [] adjList;
    delete [] prevShard;
    delete [] vecMove;
    
    for(int i=0;i<partitions;i++){
        delete [] sortedCountIJ[i];
    }
    delete [] sortedCountIJ;
    
    //remove dangling pointers
    shard=nullptr;
    adjList=nullptr;
    prevShard=nullptr;
    vecMove=nullptr;
    sortedCountIJ=nullptr;
}
