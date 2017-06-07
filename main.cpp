//
//  main.cpp
//  Boost
//
//  Created by Steve DengZishi on 6/6/17.
//  Copyright © 2017 Steve DengZishi. All rights reserved.
//

#include <iostream>
#include <boost/graph/adjacency_list.hpp>

using namespace boost;

adjacency_list<> g;//default adjacency list using vecS

typedef adjacency_list<>::vertex_descriptor Vert;
typedef adjacency_list<>::vertex_iterator VertItr; //idx in vecS , ptr in listS
typedef adjacency_list<>::edge_descriptor Edge; // (a,b), in setS repeated edge is not allowed

int main(){
    //add vertices
    Vert v1 = add_vertex(g);    //vertex_descritor is using index of vecS
    Vert v2 = add_vertex(g);
    Vert v3 = add_vertex(g);
    Vert v4 = add_vertex(g);
    
    std::cout<<v1<<" "<<v2<<" "<<v3<<" "<<v4<<std::endl;
    
    //get vertices
    std::pair<VertItr,VertItr> p = vertices(g);

    for(auto it=p.first; it!= p.second; ++it){
        std::cout<<*it<<std::endl;
    }
    //add edges
    std::pair<Edge,bool> result;
    
    result = add_edge(v1,v2,g);
    std::cout<<result.first<<" "<<result.second<<std::endl;
    result = add_edge(v2,v3,g);
    std::cout<<result.first<<" "<<result.second<<std::endl;
    result = add_edge(v3,v4,g);
    std::cout<<result.first<<" "<<result.second<<std::endl;
    result = add_edge(v4,v1,g);
    std::cout<<result.first<<" "<<result.second<<std::endl;
    //adding repeated edges
    result = add_edge(v1,v2,g);
    std::cout<<result.first<<" "<<result.second<<std::endl;
}


