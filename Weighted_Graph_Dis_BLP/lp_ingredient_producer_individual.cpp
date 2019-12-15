//
//  lp_ingredient_producer_individual.cpp
//  produce linear programs ingredients for all nodes as individual
//  and gives integer solutions
//  Boost
//
//  Created by Steve DengZishi on 1/15/19.
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
int* mass;
int* nodesTranslation;//map nodes to its pivot if it a node in the community
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
    cerr<<"In produceSortedCountIJ"<<endl;
    for(int i=0;i<nodes;i++){
        //the position (shardID) of the current node
        int from=prevShard[i];
        
        //filter out the top 3 gains movement using priority queue, and assign them with probabilities 50%, 30%, 20%
        priority_queue<pair<int,int>> top_gain_queue;
        
        for(int to=0;to<partitions;to++){
            //for any other shard other than the current node
            if(from!=to){
                //neighbors at other shard - neighbors at current shard
                int incr_cnt=neighbors[i][to]-neighbors[i][from];
                //only put positive gain into the queue
                if(incr_cnt>0){
                    top_gain_queue.emplace(incr_cnt,to);
                }
            }
        }
        
        //random integer from 0-99, (0-49) first move option, (50-79) second option, (80-99) third option
        int choice;
        int rand_num = rand()%100;
        //cerr<<"The random number is: "<<rand_num<<endl;
        if(rand_num<=49) choice=1;
        else if(rand_num<=79) choice=2;
        else choice=3;
        
        //cerr<<"The chosen choice is: "<<choice<<endl;
        
        //get the top move option according to probability
        pair<int,int> best_move(0,0);
        while(choice>0 && !top_gain_queue.empty()){
            best_move=top_gain_queue.top();
            //cerr<<"Choice is : "<<choice<<" . Best move now is : "<<best_move.first<<" "<<best_move.second<<endl;
            top_gain_queue.pop();
            choice--;
        }
        //check whether there is a effective move option, put directly into sortedcountIJ
        if (best_move.first>0 && best_move.second!=from){
            sortedCountIJ[from][best_move.second].emplace_back(best_move.first, i);
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

//count how many nodes would like to move (gain in colocation count per node > 0), count with total node weight
int countP(int i, int j){
    int cnt=0;
    FOR(k,0,sortedCountIJ[i][j].size()){
        if(sortedCountIJ[i][j][k].first>0.0) cnt+=1;
        else break;
    }
    return cnt;
}

void printCountPIJ(){
    FOR(i,0,Pcount.size()){
        cout<<Pcount[i]<<endl;
    }
}

// each dataset contains data of a piece-wise linear equation
// where a is the gradient, sum is the y (total utility gain), no is the x (number of movement)
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

//For each to from pair, print the linear information required for equations
void printLinearInfo(int i,int j){
    vecD.clear();
    //k is the count of linear piece in each ij from to pairs
    int a=0;
    int sum=0;
    int num=0;
    
    if(sortedCountIJ[i][j].size()>0) a=sortedCountIJ[i][j][0].first;
    
    //ordered (gain, nodeID) in sortedCountIJ
    FOR(z,0,sortedCountIJ[i][j].size()){
        //if(sortedCountIJ[i][j][z].first<=0) break;
        //from the second term
        if(z>0){
            //decide when to change gradient(a) on a linear piece
            //if this gain has a change in gradient, and the previous gain > 0
            //record previous linear function
            if(sortedCountIJ[i][j][z].first<a && sortedCountIJ[i][j][z-1].first>0){
                vecD.emplace_back(a,sum,num); // the index will be k-1
                a=sortedCountIJ[i][j][z].first;
                if(a<0) break;
            }
        }
        //no matter gradient is changed or not, sum is accumulated
        //add to total sum and count
        sum+=a;
        num++;
        
        //if this term is the last one
        if(z==sortedCountIJ[i][j].size()-1){
            //if gradient is positive
            if(a>0) vecD.emplace_back(a,sum,num);
        }
    }
    
    cout<<vecD.size()<<endl;
    
    FOR(j,0,vecD.size()){
        printf("%d %d %d\n",vecD[j].a,vecD[j].sum,vecD[j].no);
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

//start of main()
int main(int argc, const char * argv[]) {
    
    //get stdin from shell script
    fileName=argv[1];
    partitions=atoi(argv[2]);
    
    inFile.open(fileName,ios::in);
    
    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    
    //read number of nodes and edges
    inFile>>nodes>>edges;
    cout<<nodes<<" "<<partitions<<endl;//(lp_ingredient)
    
    //allocate memory for adjList & sortedCountIJ on the heap
    shard=new vector<int>[partitions];
    prevShard=new int[nodes];
    adjList=new vector<int>[nodes];
    weighted_adjList=new unordered_map<int,int>[nodes];
    score=new int[partitions];
    mass=new int[nodes];
    
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
    //cerr<<"after creating ADJ"<<endl;
    //    printADJ();
    inFile.close();
    
    //load previous shard[partitions] & prevShard[nodes]
    loadShard();
    //cerr<<"after loading shard"<<endl;
    //    printShard();
    
    //create neighbor list
    //    printADJ();
    createNeighborList();
    //    printNeighborList();
    
    //calculate, sort and print the increase in colocation count for all nodes moving from i to j
    //in the form (INC(increase in colocation),nodeID)
    
    //previous:
    //sort all movement options for all nodes in i shard, and keep the only highest scoring destination
    //to eliminate repeated movement falls in top x options
    //1.mapping
    //2.resize to top gain movement
    //3.reconstruct sortedCountIJ
    //reduced version of producing sortedCountIJ
    
    //new:
    //directly produce SortedCountIJ by looping all nodes and selecting top gain movements
    produceSortedCountIJ();
    
    FOR(i,0,partitions){
        FOR(j,0,partitions){
            if(i!=j){
                //sortedIJ only left with 1 top gain moving option
                //the results are sorted by gain per node
                sort(ALL(sortedCountIJ[i][j]),Greater());
                //                printSortedCount(i,j);
                //create the upper bound for number of nodes that would like to move with a positive gain per node
                Pcount.push_back(countP(i,j));
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
    delete [] weighted_adjList;
    delete [] prevShard;
    delete [] score;
    delete [] mass;
    delete [] nodesTranslation;
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
    mass=nullptr;
    nodesTranslation=nullptr;
    blocks=nullptr;
    return 0;
}
