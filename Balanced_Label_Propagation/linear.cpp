//
//  linear.cpp
//  Boost
//
//  Created by Steve DengZishi on 6/29/17.
//  Copyright © 2017 Steve DengZishi. All rights reserved.
//

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
        int diff=size[i]-lowerBound;
        printf("%d;\n",diff);
    }
}

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
        int diff=size[i]-upperBound;
        printf("%d;\n",diff);
    }
}

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

void printIJK(){
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                int k; cin>>k; // k is the different number of linear segments
                for(int h=0;h<k;h++){
                    string string_i=convertToThreeDigit(i);
                    string string_j=convertToThreeDigit(j);
                    
                    int a;cin>>a; // a is the gradient of each segments
                    int b, sum, num;
                    cin>>sum>>num;
                    b=sum-num*a; // b is the y-intercept
//                    printf("-%d x%d%d +z%d%d <= %d;\n",a,i,j,i,j,b);
                    cout<<"-"<<a<<" "<<"x"<<string_i<<string_j<<" "<<"+z"<<string_i<<string_j<<" <= "<<b<<";"<<endl;
                }
            }
        }
    }
}

int main(){
    //optimize iostream
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    cin>>nodes>>shard;
//    cout<<nodes<<" "<<shard<<endl;
    
    //dynamically allocate memory
    size=new int[shard];
    
    printObjectiveFunc("max");
    
    printIJK();
    printLXP();
    printRXP();
    printLC();
    printUC();
    
    //clean dynamically allocated memory
    delete [] size;
    return 0;
}
