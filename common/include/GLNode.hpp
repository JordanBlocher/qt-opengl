#ifndef GLNODE_H
#define GLNODE_H

#include <string>
#include <memory>
#include <list>

#include "GLStruct.hpp"

class GLNode
{
 
 public:
    GLNode();
    GLNode(const char*);
    GLNode(const std::shared_ptr<GLNode>);
    ~GLNode();

    GLuint getId() const;
    std::string getName() const;

protected:
    virtual bool Add(std::shared_ptr<GLNode>);
    virtual bool Remove(std::shared_ptr<GLNode>);
    virtual bool Status();

    GLuint id;
    GLenum type;
    GLint status;
    std::string msg;
    std::string name;

    std::list<std::shared_ptr<GLNode>> children;
};

#endif 

