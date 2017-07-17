//
//  lp_ingredient_producer.cpp
//  Boost
//
//  Created by Steve DengZishi on 7/10/17.
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
vector<PII>** sortedCountIJ;
vector<int> Pcount;
int partitions;
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

// returns the colocation count of a nodeID in certain column of shard
int colocation(int val,int col){
    int count=0;
    //O(VE)
    for(int i=0;i<shard[col].size();i++){
        for(int j=0;j<adjList[val].size();j++){
            if(shard[col][i]==adjList[val][j]){
                count++;
            }
        }
    }
    return count;
}

void printSortedCount(int i, int j){
    printf("The increase in colocation from shard %d to %d is: ",i,j);
    for(int k=0;k<sortedCountIJ[i][j].size();k++){
        printf(" (%d,%d) ",sortedCountIJ[i][j][k].first,sortedCountIJ[i][j][k].second);
    }
    cout<<endl<<endl;
}

void fSort(int i, int j){
    for(int k=0;k<shard[i].size();k++){
        //Calculate the increase in count (INC)
        int INC=colocation(shard[i][k],j)-colocation(shard[i][k],i);
        pair<int,int> p(INC,shard[i][k]);
        sortedCountIJ[i][j].push_back(p);
    }
    sort(ALL(sortedCountIJ[i][j]),Greater());
    //    printSortedCount(i,j);
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

//count how many nodes would like to move (gain in colocation count >=1)
int countP(int i, int j){
    int cnt=0;
    FOR(k,0,sortedCountIJ[i][j].size()){
        if(sortedCountIJ[i][j][k].first>=1) cnt++;
        else break;
    }
    return cnt;
}

void printCountPIJ(){
    FOR(i,0,Pcount.size()){
        cout<<Pcount[i]<<endl;
    }
}

struct dataset{
    int a;
    int sum;
    int no;
    
    dataset(int gradient, int s, int n){
        a=gradient;
        sum=s;
        no=n;
    }
};

vector<dataset> vecD;

void printLinearInfo(int i,int j){
    vecD.clear();
    int k=0;
    int sum=0;
    int a=INFINITY;
    int num=0;
    
    FOR(z,0,sortedCountIJ[i][j].size()){
        if(sortedCountIJ[i][j][z].first<=0) break;
        
        else if(sortedCountIJ[i][j][z].first!=a && sortedCountIJ[i][j][z].first>0){
            a=sortedCountIJ[i][j][z].first;
            k++;
            vecD.emplace_back(a,a+sum,num+1); // the index will be k-1
        }
        
        sum+=a;
        num++;
    }
    
    if(vecD.size()!=0) cout<<vecD.size()<<endl;
    
    FOR(j,0,k){
        printf("%d %d %d\n",vecD[j].a,vecD[j].sum,vecD[j].no);
    }
}


// Stores the first choices of all nodes after each iteration
vector<PII>* vecMove; // move[nodeID] -> vectors of (gain,destination); //only sorted and leave other options for 2nd moves
int* prevShard;

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
    for(int i=0;i<partitions;i++){
        for(int j=0;j<partitions;j++){
            //sortedCount after cut
            for(int k=0;k<sortedCountIJ[i][j].size();k++){
                vecMove[sortedCountIJ[i][j][k].second].emplace_back(sortedCountIJ[i][j][k].first,j);//(gain,destination)
            }
        }
    }
    //sort to select the top gain moving option
    for(int i=0;i<nodes;i++){
        sort(ALL(vecMove[i]),Greater());
    }
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

//start of main()
int main(int argc, const char * argv[]) {
    
    //optimize iostream
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    //get stdin from shell script
    cin>>fileName;
    cin>>partitions;
    
    inFile.open(fileName,ios::in);
    
    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    
    //read number of nodes and edges
    inFile>>nodes>>edges;
    cout<<nodes<<" "<<partitions<<endl;//(lp_ingredient)
    
    //allocate memory for vecMove, adjList & sortedCountIJ on the heap
    shard=new vector<int>[partitions];
    prevShard=new int[nodes];
    vecMove=new vector<PII>[nodes];
    adjList=new vector<int>[nodes];
    
    sortedCountIJ=new vector<PII>* [partitions];
    for(int i=0;i<partitions;i++){
        sortedCountIJ[i]=new vector<PII> [partitions];
    }
    
    //create adjacency list from edge list
    createADJ();
    //    printADJ();
    inFile.close();
    
    //load previous shard[partitions] & prevShard[nodes] 
    loadShard();
//    printShard();
    
    //calculate, sort and print the increase in colocation count for all nodes moving from i to j
    //in the form (INC(increase in colocation),nodeID)
    for(int i=0;i<partitions;i++){
        for(int j=0;j<partitions;j++){
            if(i!=j) fSort(i,j);
        }
    }
    //sort all movement options for all nodes in i shard, and keep the only highest scoring destination
    //to eliminate repeated movement falls in top x options
    //1.mapping
    //2.resize to top gain movement
    //3.reconstruct sortedCountIJ
    mapToMove();
    for(int i=0;i<nodes;i++){
        vecMove[i].resize(1);
    }
    //    printVecMove();
    
    clearSortedCount();
    
    FOR(i,0,nodes){
        //after resizing vecMove only contains top gain movement
        int dest=vecMove[i][0].second;
        int gain=vecMove[i][0].first;
        int origin=prevShard[i];
        sortedCountIJ[origin][dest].emplace_back(gain,i);
    }
    
    FOR(i,0,partitions){
        FOR(j,0,partitions){
            if(i!=j){
                sort(ALL(sortedCountIJ[i][j]),Greater());
                //                printSortedCount(i,j);
                if(countP(i,j)!=0) Pcount.push_back(countP(i,j));
                printLinearInfo(i,j);//(lp_ingredient)
            }
        }
    }
    
    //print out the number of nodes wanted to move line by line
    printCountPIJ();//(lp_ingredient)
    printShardSize();//(lp_ingredient)
    
    //fprint structured data to files to be reload later in applyMove
    FILE* outFile;
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
    delete [] prevShard;
    delete [] vecMove;
    
    for(int i=0;i<partitions;i++){
        delete [] sortedCountIJ[i];
    }
    delete [] sortedCountIJ;
    
    return 0;
}
