#ifndef GLNODE_H
#define GLNODE_H

#include <string>
#include "GLStruct.hpp"

class GLNode
{

public:
    GLNode();
    ~GLNode();

    GLuint getId() const;

    virtual bool Status();
    virtual void Add(GLNode);
    virtual void Remove(GLNode);

protected:
    GLuint id;
    GLenum type;
    GLint status;
    std::string msg;
    
};

#endif 

