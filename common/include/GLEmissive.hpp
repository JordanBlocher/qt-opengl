#ifndef GLEMISSIVE_H
#define GLEMISSIVE_H

#include "GLNode.hpp"

class GLEmissive : public GLNode
{

 public:
    GLEmissive(const char*);
    ~GLEmissive();
    
    Lights lights;

protected:
};

#endif 

