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
#include <fstream>


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
string fileName;
fstream inFile;
int nodes;
int edges;
vector<int>* adjList;

void createADJ(){
    int from,to;
    while(inFile>>from>>to){
        adjList[from].push_back(to+1);  // use to/from + 1 is to let node start from 1 instead of 0 to fit METIS format
        adjList[to].push_back(from+1);  // comment out this line if the graph is directed
        // count number of edges and nodes from edge list file
    }
}

void printADJ(){
    for(int i=0;i<nodes;i++){
//        cout<<i<<": ";
        for(int j=0;j<adjList[i].size();j++){
            cout<<adjList[i][j]<<" ";
        }
        cout<<endl;
    }
//    cout<<endl;
}

//start of main()
int main(int argc, const char * argv[]) {
    
    //optimize iostream
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    //read nodes and edges
    cin>>fileName;
    
    inFile.open(fileName,ios::in);
    
    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    
    inFile>>nodes>>edges;
    
    //allocate dynamic memory
    adjList=new vector<int>[nodes];
    
    //create adjacency list from edge list
    createADJ();
    cout<<nodes<<" "<<edges<<endl;
    printADJ();
    
    return 0;
}
