//
//  mapreduce.h
//  Boost
//
//  Created by Steve DengZishi on 6/9/17.
//  Copyright © 2017 Steve DengZishi. All rights reserved.
//

#ifndef __MAPREDUCE_H__
#define __MAPREDUCE_H__

#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "writer.h"

/* User map function typedef. Must accept key and value as strings */
typedef void (*mapFunc)(std::string key, std::string value, MapperWriter &out);
/* User reduce function typedef. Must accept key and value as strings */
typedef void (*redFunc)(std::string key, std::vector<std::string> values, ReducerWriter &out);

/* map and reduce */
void mapper(const long numReducers, const char* filename, std::string jobname, mapFunc map);
void reducer(const char* filename, std::string jobname, redFunc reduce);

/* String splitting functions */
std::vector<std::string> split(const std::string &s, char delim);

#endif /* __MAPREDUCE_H__ */
