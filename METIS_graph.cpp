//
//  METIS_graph.cpp
//  Boost
//
//  Created by Steve DengZishi on 6/27/17.
//  Copyright © 2017 Steve DengZishi. All rights reserved.
//

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
#define nodesN 4039;
int num_nodes;
int num_edges;
vector<int> adjList[4039];


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

void createADJ(){
    int from,to;
    int maxID=0;
    int edges=0;
    while(cin>>from>>to){
        adjList[from].push_back(to);
        // comment out the following line if the graph is directed
        adjList[to].push_back(from);
        // count number of edges and nodes from edge list file
        edges++;
        if(maxID<max(from,to)) maxID=max(from,to);
    }
    num_nodes=maxID;
    num_edges=edges;
}

void printADJ(){
    for(int i=0;i<4039;i++){
//        cout<<i<<": ";
        for(int j=0;j<adjList[i].size();j++){
            cout<<adjList[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}

//start of main()
int main(int argc, const char * argv[]) {
    
    //optimize iostream
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    //to get number of nodes from data file
    //    numNode();
    
    //create adjacency list from edge list
    createADJ();
    cout<<num_nodes<<" "<<num_edges<<endl;
    printADJ();
    
    return 0;
}
