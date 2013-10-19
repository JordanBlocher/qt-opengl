#ifndef GLNODE_H
#define GLNODE_H

#include <GL/glew.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "GLStruct.hpp"

class GLNode
{
 
 public:
    GLNode();
    GLNode(const char*);
    GLNode(const char*, GLenum);
    GLNode(const std::shared_ptr<GLNode>);
    ~GLNode();

    GLuint getId() const;
    std::string getName() const;
    bool Add(std::shared_ptr<GLNode>);
    bool Remove(std::shared_ptr<GLNode>);
 
protected:
    virtual bool Status();

    GLuint id;
    GLenum type;
    GLint status;
    std::string msg;
    std::string name;

    std::vector<std::shared_ptr<GLNode>> children;
};

#endif 

