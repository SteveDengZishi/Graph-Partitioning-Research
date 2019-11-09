//
//  Clus.cpp
//  Boost
//
//  Clustering algorithm without filters
//  clustering.cpp apply filters to the communities while clus.cpp writing all communities to file
//
//  Created by Steve DengZishi on 9/13/18.
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
#include <fstream> // To use c++ input and output to files
#include <time.h>
#include <stdlib.h>
#include <boost/math/special_functions/digamma.hpp>

using namespace std;

#define FOR(i,a,b) for(size_t i=a;i<b;i++)
#define DIGAMMA boost::math::digamma
//global variables here
vector<int>* blocks;
vector<int> blockSize;
//check whether it is initialized
int* prevShard;
int* blockIntEdgeCount;
int* blockExtEdgeCount;
double* density; // defined as the ratio of number of internal edges/external edges
vector<int>* adjList;
//int** adjMatrix;
//double* pi_weights;
fstream inFile;
string fileName;
int block_num;
int64_t nodes;
int64_t edges;
int iterations;
double J;
double JL;
double* h;
string verbose;
int seed;

void randomAssignment(){
    srand(seed);
    //random sharding according using a integer hash then mod 8 to distribute to shards
    for(int i=0;i<nodes;i++){
        //        cout<<"i is: "<<i<<endl;
        int block_assignment = rand() % block_num;
        //        cout<<"shard ID is: "<<shardID<<endl;
        blocks[block_assignment].push_back(i);
        //mark prevShard for the 1st random Sharding
        prevShard[i] = block_assignment;
    }
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
    printf("\nThere are %d local edges out of total %d edges, fraction of local edges is: %lf\n",localEdge,totalEdge,fraction);
    return fraction;
}

//build adjacency list from streaming edges
void createADJ(){
    //create data structures with variable size on the heap
    adjList = new vector<int>[nodes];
    
    int from,to;
    while(inFile>>from>>to){
        adjList[from].push_back(to);
        // comment out the following line if the graph is directed
        adjList[to].push_back(from);
    }
}

//convert adjacency list to adjacency matrix
//it is probably too costly on large graphs
/*
 void convertADJtoADJMatrix(){
 //dynamically initialize 2d array to 0s
 adjMatrix = new int*[nodes];
 
 FOR(i, 0, nodes){
 adjMatrix[i] = new int[nodes]{0};
 }
 
 //scan through adjList and update edges to matrix
 FOR(i,0,nodes){
 FOR(j,0,adjList[i].size()){
 adjMatrix[i][adjList[i][j]]=1;
 }
 }
 }
 */
//replace adjMatrix using adjList in checking edge connection
int check_connection(int i, int j){
    FOR(k,0,adjList[i].size()){
        if(adjList[i][k] == j) return 1;
    }
    return 0;
}

//start with edges in the adjacency list and check whether they have the same communities assignments
int check_communities(int i, int j){
    if(prevShard[i]==prevShard[j]) return 1;
    else return 0;
}

//m++, c+
//deprecated as it is too slow for large graphs to use O(V^2+E) method
/* int countEdgesWithinComm(){
 int count = 0;
 FOR(i,0,nodes){
 FOR(j,0,nodes){
 //printf("%d %d ", i,j);
 if(i>j){
 if(prevShard[i]==prevShard[j] && check_connection((int)i,(int)j)==1){
 count++;
 }
 }
 }
 }
 cout<<"Total number of edges within communities are: "<<count<<endl;
 return count;
 } */

//SUM from 1 to K |E(Bk, Bk)|
//deprecated as it is still slow for large graphs O(V^2/K+E)
/* int countEdgesWithinComm(){
 int count=0;
 FOR(i,0,block_num){
 FOR(j,0,blocks[i].size()){
 FOR(k,0,blocks[i].size()){
 //undirected graph only count in one direction
 if(blocks[i][j]>blocks[i][k]){
 if(check_connection(blocks[i][j], blocks[i][k])) count++;
 }
 }
 }
 }
 cout<<"Total number of edges within communities are: "<<count<<endl;
 return count;
 } */

//this algorithm takes only O(V+E) by traversing through adjList
int64_t countEdgesWithinComm(){
    int64_t count=0;
    FOR(i,0,nodes){
        FOR(j,0,adjList[i].size()){
            if(check_communities(i,adjList[i][j])) count++;
        }
    }
    count = count/2;
    cout<<"Total number of edges within communities are: "<<count<<endl;
    return count;
}

//m+-, c-
/* int countNonEdgesWithinComm(){
 int count = 0;
 FOR(i,0,nodes){
 FOR(j,0,nodes){
 if(i>j){
 if(prevShard[i]==prevShard[j] && check_connection((int)i,(int)j)==0){
 count++;
 }
 }
 }
 }
 cout<<"Total number of non-edges within communities are: "<<count<<endl;
 return count;
 } */

//SUM from 1 to k (nk(nk-1)/2) - m++
int64_t countNonEdgesWithinComm(int64_t mpp){
    int64_t count=0;
    FOR(i,0,block_num){
        if(blockSize[i]!=0) count += blockSize[i]*(blockSize[i]-1)/2;
    }
    count=count-mpp;
    cout<<"Total number of non-edges within communities are: "<<count<<endl;
    return count;
}

//m-+, d+
//deprecated
/* int countEdgesBetweenComm(){
 int count = 0;
 FOR(i,0,nodes){
 FOR(j,0,nodes){
 if(i>j){
 if(prevShard[i]!=prevShard[j] && check_connection((int)i,(int)j)==1){
 count++;
 }
 }
 }
 }
 cout<<"Total number of edges between communities are: "<<count<<endl;
 return count;
 } */

//m-m++
int64_t countEdgesBetweenComm(int64_t mpp){
    int64_t count = edges-mpp;
    cout<<"Total number of edges between communities are: "<<count<<endl;
    return count;
}

//m--, d-.
//deprecated
/* int countNonEdgesBetweenComm(){
 int count = 0;
 FOR(i,0,nodes){
 FOR(j,0,nodes){
 if(i>j){
 if(prevShard[i]!=prevShard[j] && check_connection((int)i,(int)j)==0){
 count++;
 }
 }
 }
 }
 cout<<"Total number of non-edges between communities are: "<<count<<endl;
 return count;
 } */

//n(n-1)/2-m-m-+
int64_t countNonEdgesBetweenComm(int64_t mmp){
    int64_t count=0;
    if(nodes!=0) count = nodes*(nodes-1)/2-edges-mmp;
    cout<<"Total number of non-edges between communities are: "<<count<<endl;
    return count;
}

//nµ, nk
void countBlockSize(){
    blockSize.clear();
    FOR(i,0,block_num){
        blockSize.push_back(blocks[i].size());
    }
}

void printBlockSize(){
    cout<<"\nPrinting blocks sizes:\n";
    FOR(i,0,block_num){
        cout<<blockSize[i]<<" ";
    }
    cout<<endl;
}

/* void generate_pi_weights(){
 int sum = 0;
 srand(seed);
 for(int i=0;i<block_num;i++){
 pi_weights[i] = rand()%1000;
 sum+=pi_weights[i];
 }
 for(int i=0; i<block_num; i++){
 pi_weights[i]/=sum;
 }
 }
 
 void print_pi_weights(){
 cout<<"The pi_weights are: "<<endl;
 for(int i=0;i<block_num;i++){
 cout<<pi_weights[i]<<" ";
 }
 cout<<endl<<endl;
 }
 
 int get_block_assignment(){
 //generate a random double between 0~1
 double r = (double) rand()/(RAND_MAX);
 //printf("r number is: %f\n",r);
 double pi_sum=0.0;
 for(int i=0;i<block_num;i++){
 pi_sum+=pi_weights[i];
 if(pi_sum>=r){
 return i;
 }
 }
 return block_num;
 } */

void print_blocks_assignments(){
    for(int i=0;i<block_num;i++){
        cout<<"In block "<<i<<endl;
        for(int j=0;j<blocks[i].size();j++){
            cout<<blocks[i][j]<<" ";
        }
        cout<<endl;
    }
}

//count the number of edges between node y and block z
//it is slow when clusters are super large
int countEdgesBetweenNodeAndBlock(int y, int z){
    int count = 0;
    //each of the nodes in block[z] check it adjMatrix with node y whether edge exists
    FOR(i,0,blocks[z].size()){
        int node = blocks[z][i];
        if(check_connection(y,node)) count++;
    }
    return count;
}

//using discounted vote to select the most probable block assignment for each node
int findBestAssignmentK(int i,double J, double JL, double* h){
    
    vector<pair<int,double>> results;
    unordered_set<int> target_shards;
    
    //calculate weights for every blocks, it can be slow, deprecated
    /*  FOR(j,0,block_num){
     //count wk = |E(i,Bk)| + |E(Bk,i)|
     int w_k = countEdgesBetweenNodeAndBlock(i,(int)j);
     //weigh on the block sizes
     int effective_size_k = blockSize[j];
     if(prevShard[i]==(int)j) effective_size_k = blockSize[j] - 1;
     
     double val = J*w_k - JL*effective_size_k - h[j];
     //cout<<val<<" ";
     results.push_back(val);
     } */
    
    //only calculate weights for blocks that has adjacent nodes
    //1.gather which blocks have the adjacent nodes, use unordered_set to remove duplicates
    FOR(j,0,adjList[i].size()){
        target_shards.insert(prevShard[adjList[i][j]]);
    }
    
    //2.loop through those blocks with adjacent nodes and ignore others to generate results pairs
    for(auto itr=target_shards.begin(); itr!=target_shards.end(); itr++){
        //count wk = |E(i,Bk)| + |E(Bk,i)|
        int w_k = countEdgesBetweenNodeAndBlock(i, *itr);
        //cerr<<"edges are: "<<w_k<<endl;
        //weigh on the block sizes
        int effective_size_k = blockSize[*itr];
        if(prevShard[i]==*itr) effective_size_k = blockSize[*itr] - 1;
        
        double val = J*w_k - JL*effective_size_k - h[*itr];
        //putting corresponding block index and value into vector of pairs
        results.emplace_back(*itr, val);
    }
    
    //return the max resulting index of the results
    //if results vector has no entry, means this node is a lone node
    if(results.size()==0) return prevShard[i];
    
    int max_idx=0;
    FOR(j,1,results.size()){
        if(results[j].second > results[max_idx].second) max_idx=j;
    }
    return results[max_idx].first;
}

//internal edges/external edges is a better ratio than edge density
void showEdgeDensity(){
    //initialize or clear block edge count
    FOR(k,0,block_num){
        blockIntEdgeCount[k]=0;
        blockExtEdgeCount[k]=0;
        density[k]=0.0;
    }
    //traverse adjacency list and add counts
    FOR(i,0,nodes){
        FOR(j,0,adjList[i].size()){
            //undirected graph prevent double counting for internal edges
            //if they are in the same community
            if(adjList[i][j]>i && prevShard[i]==prevShard[adjList[i][j]]) blockIntEdgeCount[prevShard[i]]++;
            //if they are in different communities, need to double count when two communities share external edges
            else if(prevShard[i]!=prevShard[adjList[i][j]]) blockExtEdgeCount[prevShard[i]]++;
        }
    }
    //print block size and edge density
    FOR(l,0,block_num){
        if(blockSize[l]>0 && blockExtEdgeCount[l]>0) {
            density[l] = (double)blockIntEdgeCount[l]/(double)blockExtEdgeCount[l];
            printf("(%d, %.3f) ",blockSize[l],density[l]);
        }
    }
    cout<<endl;
}

//save all clusters except those of size 0
void saveAllComm(){
    
    //write into files "clusters.txt" in the following format
    //eligible_block_num
    //size_1 nodeID_1, nodeID_2,....
    //size_2 nodeID_1, nodeID_2,....
    //...in total block_num lines follows....
    FILE* outFile;
    string clusterFile = fileName.substr(0,fileName.find(".")) + "_clusters.txt";
    outFile=fopen(clusterFile.c_str(), "wb");
    
    //filter out suitable communities
    vector<int> block_indexes;
    
    FOR(i,0,block_num){
        if(blockSize[i]>0){
            block_indexes.push_back(i);
        }
    }
    
    cout<<"After filtering out 0 sized clusters, there are "<<block_indexes.size()<<" number of eligible communities."<<endl;
    
    //writing to file
    fprintf(outFile,"%d", int(block_indexes.size()));
    FOR(i,0,block_indexes.size()){
        fprintf(outFile,"\n%d ", int(blocks[block_indexes[i]].size()));
        FOR(j,0,blocks[block_indexes[i]].size()){
            fprintf(outFile,"%d ", blocks[block_indexes[i]][j]);
        }
    }
    fclose(outFile);
}

void reConstructBlocks(){
    //clear shard
    FOR(i,0,block_num){
        blocks[i].clear();
    }
    FOR(i,0,nodes){
        blocks[prevShard[i]].push_back((int)i);
    }
}

//print weights J, JL and h
void printWeights(){
    cout<<"\nPrinting calculated weights..."<<endl;
    cout<<"The value of J is: "<<J<<endl;
    cout<<"The value of JL is: "<<JL<<endl;
    cout<<"The values of h are: ";
    FOR(i,0,block_num){
        cout<<h[i]<<" ";
    }
    cout<<endl;
}

//prior values for a+0, b+0, a-0, b-0, vec{n0}
double ap0=3;
double bp0=1;
double am0=1;
double bm0=3;

double* vecN;

//start of main program
//input: filename, block_num, (optional) verbose flag v
int main(int argc, const char * argv[]){
    
    //get stdin from shell script
    fileName=argv[1];
    seed=atoi(argv[2]);
    iterations=0;
    
    if(argc==4) verbose=argv[3];
    
    //Reading from graphs
    inFile.open(fileName,ios::in);
    
    if(!inFile){
        cerr<<"Error occurs while opening the file"<<endl;
        exit(1);
    }
    
    //read number of nodes and edges
    inFile>>nodes>>edges;
    
    //block_num = 1/5 nodes;
    block_num=nodes/5;
    //initialize n using block numbers
    vecN = new double[block_num];
    FOR(i,0,block_num){
        vecN[i]=1.0;
    }
    
    //produce adjList && adjMatrix
    createADJ();
    //convertADJtoADJMatrix();
    
    //initialize prevShard for O(1) block assignment query
    prevShard = new int[nodes];
    blockIntEdgeCount = new int[block_num];
    blockExtEdgeCount = new int[block_num];
    density = new double[block_num];
    
    //Bayesian Approach to identify block structure in the network
    //Variational method for approximate inference
    //Variant of BLP which takes a discounted vote over neighbors membership
    //1.Randomly initialize the block assignments
    //pi_weights = new double[block_num];
    blocks = new vector<int>[block_num];
    randomAssignment();
    countBlockSize();
    printBlockSize();
    double locality=printLocatlityFraction();
    printf("After random assignments, the locality is: %lf\n\n", locality);
    
    //set a flag only to stop interations when locality converges, improvement < 1%
    bool cont = true;
    
    //repeat discounted vote process until convergence in FA[q] variational free energy
    while(cont){
        iterations++;
        printf("\nStarting clustering iteration %d.....\n", iterations);
        
        //count the number of edges from the observed network
        int64_t mpp = countEdgesWithinComm();
        int64_t mpm = countNonEdgesWithinComm(mpp);
        int64_t mmp = countEdgesBetweenComm(mpp);
        int64_t mmm = countNonEdgesBetweenComm(mmp);
        
        //calculating discounted votes
        J = DIGAMMA(mpp+ap0) - DIGAMMA(mpm+bp0) - DIGAMMA(mpm+am0) + DIGAMMA(mmm+bm0);
        JL = DIGAMMA(mmm+bm0) - DIGAMMA(mmp+am0+mmm+bm0) - DIGAMMA(mpm+bp0) + DIGAMMA(mpp+ap0+mpm+bp0);
        h = new double[block_num];
        
        int ak_sum=0;
        for(int j=0;j<block_num;j++) ak_sum+=vecN[j];
        for(int i=0;i<block_num;i++) h[i] = DIGAMMA(blocks[i].size()+vecN[i]) - DIGAMMA(ak_sum);
        
        
        if(verbose=="v"){
            //Debugging weights
            printWeights();
        }
        else{
            cout<<"\nWeights calculations are done."<<endl;
        }
        
        //sub in formula for discounted vote
        int move_cnt = 0;
        for(int i=0;i<nodes;i++){
            int new_assignment = findBestAssignmentK((int)i,J,JL,h);
            if(verbose=="v"){
                cout<<"moving node "<<i<<endl;
                cout<<"previous lies "<<prevShard[i]<<"; new assignment is "<<new_assignment<<endl;
            }
            if(prevShard[i]!=new_assignment) move_cnt++;
            prevShard[i] = new_assignment;
        }
        //    //Map blocks to shards
        //    //Or collapse nodes to use lpsolve
        //
        //    //    printShard();
        //    //output locality info to shell
        
        reConstructBlocks();
        
        //count the sizes of each block into blockSize vector
        countBlockSize();
        showEdgeDensity();
        //print_blocks_assignments();
        
        printf("There are %d nodes moved in iteration %d\n", move_cnt, iterations);
        double new_locality=printLocatlityFraction();
        double change = (new_locality - locality) / locality;
        if (change < 0.01) cont=false;
        
        //override the old locality
        locality = new_locality;
    }
    printf("After %d iterations of posterior assignments, the final locality is: %.5f\n", iterations, locality);
    
    //save filtered communities to file
    saveAllComm();
    //
    //    //write data to file for graph plotting
    //    FILE* outFile=fopen("graph_plotting_data.txt","w");
    //    fprintf(outFile,"node locality\n");
    //    fprintf(outFile,"%d %lf\n",nodes,locality);
    //    fclose(outFile);
    //
    //    //fprint to files
    //    outFile=fopen("sharding_result.bin", "wb");
    //
    //    //write block of data to stream
    //    //    fwrite(prevShard, sizeof(int), nodes, outFile);
    //    //    fwrite(shard, sizeof(vector<int>), partitions, outFile);
    //    for(int i=0;i<partitions;i++){
    //        fprintf(outFile,"%d\n",int(shard[i].size()));
    //        for(int j=0;j<shard[i].size();j++){
    //            fprintf(outFile,"%d ", shard[i][j]);
    //        }
    //        fprintf(outFile,"\n");
    //    }
    //
    //    for(int i=0;i<nodes;i++){
    //        fprintf(outFile,"%d ", prevShard[i]);
    //    }
    //
    //delete dynamic allocation
    delete [] blocks;
    delete [] prevShard;
    delete [] adjList;
    delete [] h;
    delete [] vecN;
    delete [] blockIntEdgeCount;
    delete [] blockExtEdgeCount;
    delete [] density;
    /*
     FOR(i,0,nodes){
     delete [] adjMatrix[i];
     }
     
     delete [] adjMatrix;
     */
    
    //remove dangling pointers
    blocks=nullptr;
    adjList=nullptr;
    prevShard=nullptr;
    vecN=nullptr;
    h=nullptr;
    blockIntEdgeCount=nullptr;
    blockExtEdgeCount=nullptr;
    density=nullptr;
    //adjMatrix=nullptr;
    //fclose(outFile);
    //
    return 0;
}
