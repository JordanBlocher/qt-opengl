#ifndef GLNODE_H
#define GLNODE_H

#include <string>
#include <memory>

#include "GLContext.hpp"

class GLNode
{
 
 public:
    GLNode();
    GLNode(const char*);
    GLNode(const GLNode&);
    ~GLNode();

    GLuint getId() const;
    std::string getName() const;

protected:
    virtual bool Add(std::shared_ptr<GLNode>);
    virtual bool Remove(const char*);
    virtual bool Status();

    GLuint id;
    GLenum type;
    GLint status;
    std::string msg;
    std::string name;
};

#endif 

