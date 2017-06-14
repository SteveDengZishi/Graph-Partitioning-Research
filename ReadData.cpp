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

//functions, global variables, comparators & Non-STL Data Structures definition here
#define SHARDSIZE 8;
vector<int>  arr[8];

void printShard(){
    for(int i=0;i<8;i++){
        for(int j=0;j<arr[i].size();j++){
            cout<<arr[i][j]<<" ";
        }
        cout<<endl;
    }
}

int f(int i, int j){
    
}

int colocation(int idx){
    
}

//start of main()
int main(int argc, const char * argv[]) {
    
    //optimize iostream
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    //using edge list to decide how many nodes are there
    int nodeID;
    int maxID=0;
    
    //to map nodeID to label
    unordered_map<int, int> shard;
    
    //to get number of nodes from data file
    while(cin>>nodeID){
        if(nodeID>maxID) maxID=nodeID;
    }
    
    //random sharding according to mod 8
    for(int i=0;i<maxID+1;i++){
        int shardID = i % SHARDSIZE;
        shard[i]=shardID;
        arr[shardID].push_back(i);
    }
 
    printShard();
    
    return 0;
}
