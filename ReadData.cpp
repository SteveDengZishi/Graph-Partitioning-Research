//
//  ReadData.cpp
//  Boost
//
//  Created by Steve DengZishi on 6/13/17.
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
#define SHARDSIZE 8;
#define nodesN 4039;
vector<int> shard[8];
vector<int> adjList[4039];
vector<PII> sortedCountIJ;
vector<int> Pcount;

unsigned int int_hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

void numNode(){
    int nodeID;
    int maxID=0;
    while(cin>>nodeID){
        if(nodeID>maxID) maxID=nodeID;
    }
    cout<<"The maxID is: "<<maxID<<endl;
    printf("There are %d nodes in total\n",maxID+1);
}

void printShard(){
    for(int i=0;i<8;i++){
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
    for(int k=0;k<sortedCountIJ.size();k++){
        printf(" (%d,%d) ",sortedCountIJ[k].first,sortedCountIJ[k].second);
    }
    cout<<endl<<endl;
}

void fSort(int i, int j){
    for(int k=0;k<shard[i].size();k++){
        //Calculate the increase in count (INC)
        int INC=colocation(shard[i][k],j)-colocation(shard[i][k],i);
        pair<int,int> p(INC,shard[i][k]);
        sortedCountIJ.push_back(p);
    }
    sort(ALL(sortedCountIJ),Greater());
//    printSortedCount(i,j);
}

void createADJ(){
    int from,to;
    while(cin>>from>>to){
        adjList[from].push_back(to);
        // comment out the following line if the graph is directed
        adjList[to].push_back(from);
    }
}

void printADJ(){
    for(int i=0;i<4039;i++){
        cout<<i<<": ";
        for(int j=0;j<adjList[i].size();j++){
            cout<<adjList[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}

void printShardSize(){
    FOR(i,0,8){
        cout<<shard[i].size()<<" "<<endl;
    }
}

int countP(){
    int cnt=0;
    FOR(i,0,sortedCountIJ.size()){
        if(sortedCountIJ[i].first>=1) cnt++;
        else break;
    }
    return cnt;
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

void printLinearInfo(){
    vecD.clear();
    int k=0;
    int sum=0;
    int a=INFINITY;
    int num=0;
    
    FOR(i,0,sortedCountIJ.size()){
        if(sortedCountIJ[i].first<=0) break;
        
        else if(sortedCountIJ[i].first!=a && sortedCountIJ[i].first>0){
            a=sortedCountIJ[i].first;
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

//start of main()
int main(int argc, const char * argv[]) {
    
    //optimize iostream
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    //to get number of nodes from data file
//    numNode();
    cout<<4039<<endl;
    
    //random sharding according using a integer hash then mod 8 to distribute to shards
    for(int i=0;i<4039;i++){
//        cout<<"i is: "<<i<<endl;
        unsigned hash_val = int_hash(i);
//        cout<<"hash is: "<<hash_val<<endl;
        int shardID = hash_val % SHARDSIZE;
//        cout<<"shard ID is: "<<shardID<<endl;
        shard[shardID].push_back(i);
    }
    
    //show the sharding result
//    printShard();
    printShardSize();
    
    //create adjacency list from edge list
    createADJ();
//    printADJ();
    
    //calculate, sort and print the increase in colocation count for all nodes moving from i to j
    //in the form (INC(increase in colocation),nodeID)
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            sortedCountIJ.clear();
            if(i!=j) fSort(i,j);
            if(countP()!=0) Pcount.push_back(countP());
            printLinearInfo();
        }
    }
    
    FOR(i,0,Pcount.size()){
        cout<<Pcount[i]<<endl;
    }
    
    return 0;
}
