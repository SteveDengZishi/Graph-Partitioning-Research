//
//  linear.cpp
//  Boost
//
//  Created by Steve DengZishi on 6/29/17.
//  Copyright © 2017 Steve DengZishi. All rights reserved.
//

#include <iostream>

using namespace std;

int shard=8;
int nodes;
int size[8];
double f=0.10;

void printObjectiveFunc(string minOrMax){
    cout<<minOrMax<<":";
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                printf(" +z%d%d",i,j);
            }
        }
    }
    cout<<";"<<endl;
}

void printLC(){
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                printf("+x%d%d ",i,j);
                printf("-x%d%d ",j,i);
            }
        }
        printf(">= ");
        cin>>size[i];
        int lowerBound=(nodes/shard)*(1-f);
        int diff=lowerBound-size[i];
        printf("%d;\n",diff);
    }
}

void printUC(){
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                printf("+x%d%d ",i,j);
                printf("-x%d%d ",j,i);
            }
        }
        printf("<= ");
        int upperBound=(nodes/shard)*(1+f);
        int diff=upperBound-size[i];
        printf("%d;\n",diff);
    }
}

void printLXP(){
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j) printf("x%d%d >= 0;\n",i,j);
        }
    }
}

void printRXP(){
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                int num;
                cin>>num;
                printf("x%d%d <= %d;\n",i,j,num);
            }
        }
    }
}

void printIJK(){
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                int k; cin>>k; // k is the different number of linear segments
                for(int h=0;h<k;h++){
                    int a;cin>>a; // a is the gradient of each segments
                    int b, sum, num;
                    cin>>sum>>num;
                    b=sum-num*a; // b is the y-intercept
                    printf("-%d x%d%d +z%d%d <= %d;\n",a,i,j,i,j,b);
                }
            }
        }
    }
}

int main(){
    //change sequence of eqn to allow x01 - x76
    cin>>nodes;
    printObjectiveFunc("max");
    
    printIJK();
    printLXP();
    printRXP();
    printLC();
    printUC();
    
    return 0;
}
