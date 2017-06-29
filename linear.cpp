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

int main(){
    cout<<"max: ";
    for(int i=0;i<shard;i++){
        for(int j=0;j<shard;j++){
            if(i!=j){
                printf("+z%d%d",i,j);
            }
        }
    }
    cout<<";"<<endl;
    return 0;
}
