//
//  linear.cpp
//  Boost
//
//  Created by Steve DengZishi on 6/29/17.
//  Copyright © 2017 Steve DengZishi. All rights reserved.
//
//  This file is to take in the linear infomation produced by lp_ingredient_producer and formulate & and print them into the input format required by lp_solver

#include <iostream>

using namespace std;

int shard;
int nodes;
int* size;
double f=0.10;

//convert number to 3 digit to format lp_eqns
//we assume there are less than 1000 partitions, so maximum is 999 which has 3 number of digits, eg 1 becomes 001, 49 becomes 049
string convertToThreeDigit(int num){
    string string_i=to_string(num);
    size_t i_bit=string_i.length();
    
    for(int l=0;l<(3-i_bit);l++){
        string_i="0"+string_i;
    }
    return string_i;
}

void printObjectiveFunc(string minOrMax){
    cout<<minOrMax<<":";
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                string string_i=convertToThreeDigit(i);
                string string_j=convertToThreeDigit(j);
                
//                printf(" +z%d%d",i,j);
                cout<<" +z"<<string_i<<string_j;
            }
        }
    }
    cout<<";"<<endl;
}

//print upper partition size bound(there is a sign error on paper)
void printLC(){
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                string string_i=convertToThreeDigit(i);
                string string_j=convertToThreeDigit(j);
                
//                printf("+x%d%d ",i,j);
//                printf("-x%d%d ",j,i);
                cout<<"+x"<<string_i<<string_j<<" ";
                cout<<"-x"<<string_j<<string_i<<" ";
            }
        }
        printf("<= ");
        cin>>size[i];
        int lowerBound=(nodes/shard)*(1-f);
        int diff=0;
        if(size[i]>lowerBound){
            diff=size[i]-lowerBound;
        }
        printf("%d;\n",diff);
    }
}

//print lower size bound
void printUC(){
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                string string_i=convertToThreeDigit(i);
                string string_j=convertToThreeDigit(j);
                
//                printf("+x%d%d ",i,j);
//                printf("-x%d%d ",j,i);
                cout<<"+x"<<string_i<<string_j<<" ";
                cout<<"-x"<<string_j<<string_i<<" ";
            }
        }
        printf(">= ");
        int upperBound=(nodes/shard)*(1+f);
        int diff=0;
        if(size[i]<upperBound){
            diff=size[i]-upperBound;
        }
        printf("%d;\n",diff);
    }
}

//print left bound of move count >= 0
void printLXP(){
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j) {
                string string_i=convertToThreeDigit(i);
                string string_j=convertToThreeDigit(j);
//                printf("x%d%d >= 0;\n",i,j);
                cout<<"x"<<string_i<<string_j<<" >= "<<"0"<<";"<<endl;
            }
        }
    }
}

//print right bound of move count <= num(gain>0)
void printRXP(){
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                string string_i=convertToThreeDigit(i);
                string string_j=convertToThreeDigit(j);
              
                int num;
                cin>>num;
//                printf("x%d%d <= %d;\n",i,j,num);
                cout<<"x"<<string_i<<string_j<<" <= "<<num<<";"<<endl;
            }
        }
    }
}

//printing all linear segments and their bounds
//in -a(ijk)x(ij) + z(ij) <= b(ijk)
void printIJK(){
    //for each ij (from,to) pairs && i!=j
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                //there are k linear segments
                int k; cin>>k; // k is the different number of linear segments
                for(int h=0;h<k;h++){
                    string string_i=convertToThreeDigit(i);
                    string string_j=convertToThreeDigit(j);
                    
                    double a,b,sum; cin>>a; // a is the gradient of each segments
                    int num;
                    cin>>sum>>num;
                    b=sum-num*a; // b is the y-intercept
                    //b should not be negative or a very small number due to double precision
                    if(b<0.01) b=0;
//                    printf("-%d x%d%d +z%d%d <= %d;\n",a,i,j,i,j,b);
                    cout<<"-"<<a<<" "<<"x"<<string_i<<string_j<<" "<<"+z"<<string_i<<string_j<<" <= "<<b<<";"<<endl;
                }
            }
        }
    }
}

int main(){

    cin>>nodes>>shard;
//    cout<<nodes<<" "<<shard<<endl;
    
    //dynamically allocate memory
    size=new int[shard];
    
    //print maximizing objective function
    printObjectiveFunc("max");
    
    //printing all linear segments and their bounds
    //in -a(ijk)x(ij) + z(ij) <= b(ijk)
    printIJK();
    
    //print left bound of move count >= 0
    printLXP();
    //print right bound of move count <= num(gain>0)
    printRXP();
    
    //print upper partition size bound
    printLC();
    //print lower size bound
    printUC();
    
    //clean dynamically allocated memory
    delete [] size;
    size=nullptr;
    
    return 0;
}
