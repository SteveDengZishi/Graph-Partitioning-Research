//
//  clean.cpp
//  Boost
//
//  Created by Steve DengZishi on 7/16/17.
//  Copyright © 2017 Steve DengZishi. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream> // To use c++ input and output to files

using namespace std;
int partitions;

int main(){
//    cin>>partitions;
    string line;
//    for(int i=0;i<(4+partitions*(partitions-1));i++){
//        getline(cin,line);
//    }
    while(getline(cin,line)){
        if(line[0]=='x') cout<<line<<endl;
    }
    return 0;
}
