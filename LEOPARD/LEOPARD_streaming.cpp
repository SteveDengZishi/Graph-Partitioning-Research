//
//  LEOPARD
//
//  Created by Steve DengZishi on 7/26/17.
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
#include <fstream> //To use c++ style read stream from file

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
typedef pair<int,int> PII;
typedef vector<PII> Edges;

//debugging functions here
template<class T>
void DEVEC(vector<T> vec){
    FOR(i,0,vec.size()){
        DE(vec[i]);
    }
    cout<<endl;
}

template<class T>
void DEARRAY(T array[],int num=20){
    FOR(i,0,num+1){
        cout<<array[i]<<" ";
    }
    cout<<endl;
}

//functions, global variables, comparators & Non-STL Data Structures definition here
int partitions;
int nodes;
int edges;
vector<int>* shard;
vector<int>* adjList;
vector<PII> vecOfEdges;
string fileName;
fstream inFile;
int** neighbors;
int* score;
int* skippedComp;
int* prevShard;
bool* lookup;
double threshold=0.50;
double r=1.5;
double a;

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
        vecOfEdges.emplace_back(from,to);
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

int scoring(int nodeID, int shardID){
    //number of neighbours of nodeID in that shard - c(current size of shard)
    a=sqrt(partitions)*abs(edges)/pow(abs(nodes),r);
    int score=-a*(r/2)*pow(shard[shardID].size(),r-1);
    return score;
}

void calculateAllScores(int nodeID){
    for(int i=0;i<partitions;i++){
        score[i]=scoring(nodeID,i);
    }
}

int returnMax(int* score){
    int maximum=-INFINITY;
    int index=-1;
    for(int i=0;i<partitions;i++){
        if(score[i]>maximum) {
            maximum=score[i];
            index=i;
        }
    }
    return index;
}

int assignNode(int nodeID){
    calculateAllScores(nodeID);//calculate the scoring for all shards
    int maxShard=returnMax(score);
    shard[maxShard].push_back(nodeID);//place the node in the shard with highest score
    prevShard[nodeID]=maxShard;
    
    return maxShard;
}

bool skipping(int nodeID){
    int totalNeighbor=0;
    //calculate total number of neighbors
    FOR(i,0,partitions){
        totalNeighbor+=neighbors[nodeID][i];
    }
    skippedComp[nodeID]+=1;
    if(skippedComp[nodeID]/totalNeighbor>=threshold){
        skippedComp[nodeID]=0;
        return true;
    }
    return false;
}

void rippleEffect(int nodeID, int origin){
    //first chance it may be skipped
    bool reExamine=skipping(nodeID);
    int prev,now;
    if(reExamine){
        prev=prevShard[nodeID];
        now=assignNode(nodeID);
    }
    else return;
    //second chance if it is not skipped it may or may not change assignment
    //only recursively populate to all its neighbors if the node actually moves assignment
    if(prev!=now){
        for(int i=0;i<adjList[nodeID].size();i++){
            //prevent going back to origin and double counting
            if(adjList[nodeID][i]!=origin) rippleEffect(adjList[nodeID][i],nodeID);
        }
    }
    else return;
}

//start of main()
int main(int argc, const char * argv[]) {
    //optimize iostream
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    //reminder to clear vector between test cases
    //Your code here
    cin>>fileName;
    cin>>partitions;
    
    inFile.open(fileName,ios::in);
    if(!inFile) cerr<<"could not open file correctly"<<endl;
    
    //read number of nodes and edges
    inFile>>nodes>>edges;
    
    //allocate memory dynamically
    adjList=new vector<int>[nodes];
    shard=new vector<int>[partitions];
    score=new int[partitions];
    lookup=new bool[nodes]{false};
    skippedComp=new int[nodes]{0};
    prevShard=new int[nodes];
    
    //make sure to change neighbors number on the fly
    neighbors=new int*[nodes];
    for(int i=0;i<nodes;i++){
        neighbors[i]=new int[partitions];
    }
    
    //create adjList and vecOfEdges
    createADJ();
    
    //re-shuffle the edges to be streamed in later
    random_shuffle(vecOfEdges.begin(), vecOfEdges.end());
    
    //stream in edges in random order
    for(int i=0;i<vecOfEdges.size();i++){
        int from=vecOfEdges[i].first;
        int to=vecOfEdges[i].second;
        int shardNumFrom,shardNumTo;
        
        if(!lookup[from]){
            lookup[from]=true;//mark node as seen
            shardNumFrom=assignNode(from);
        }
        else{
            shardNumFrom=prevShard[from];//if node already exist, use the lookup table to find the shard
        }
        
        if(!lookup[to]){
            lookup[to]=true;//mark node as seen
            shardNumTo=assignNode(to);
        }
        else{
            shardNumTo=prevShard[to];
        }
        
        //start to check whether there is a need to re-examine, only check when the new edge added
        //from and to are in different shards
        if(shardNumFrom!=shardNumTo){
            //recursively run rippleEffect starting from both points
            rippleEffect(from,to);
            rippleEffect(to,from);
        }
        
    }
    
    //after streaming is done add replications
    
    
    
    return 0;
}
