//
//  writer.h
//  Boost
//
//  Created by Steve DengZishi on 6/9/17.
//  Copyright © 2017 Steve DengZishi. All rights reserved.
//

#ifndef __WRITER_H__
#define __WRITER_H__

#include <sstream>
#include <iostream>
#include <string>

class writer
{
protected:
    std::stringstream s;
    
public:
    virtual std::string str() = 0;
    virtual void reset() = 0;
};

class MapperWriter : writer
{
public:
    virtual void write(std::string key, std::string value)
    {
        s << key << "\t" << value << "\n";
    }
    
    virtual std::string str()
    {
        return s.str();
    }
    
    virtual void reset()
    {
        s.str("");
    }
};

class ReducerWriter : writer
{
public:
    virtual void write(std::string value)
    {
        s << value << "\n";
    }
    
    virtual std::string str()
    {
        return s.str();
    }
    
    virtual void reset()
    {
        s.str("");
    }
};

#endif /* __WRITER_H__ */
