#ifndef GLUNIFORM_H
#define GLUNIFORM_H

#include "GLNode.hpp"

#include <vector>

class GLUniform : public GLNode
{
 public:
    GLUniform();
    GLUniform(const char*, GLuint, GLuint, GLuint);
    GLUniform(const char*, std::vector<std::string>, GLsizeiptr, GLuint, GLuint);
    ~GLUniform();

    void setBuffer(GLuint);
    GLuint Size();
    GLuint Index();
    GLuint Offset();
    GLuint Buffer();

 private:
    std::string name;
    std::vector<std::string> attributes;
    GLuint size;
    GLuint index;
    GLuint offset;
    GLuint bufferId;

};

#endif 

