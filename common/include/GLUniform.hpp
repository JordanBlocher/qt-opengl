#ifndef GLUNIFORM_H
#define GLUNIFORM_H

#include "GLNode.hpp"

class GLUniform : public GLNode
{
 public:
    GLUniform();
    GLUniform(const char*, GLuint, GLuint, GLuint);
    ~GLUniform();

    void setBuffer(GLuint);
    GLuint Size();
    GLuint Index();
    GLuint Offset();
    GLuint Buffer();

 private:
    GLuint size;
    GLuint index;
    GLuint offset;
    GLuint bufferId;

};

#endif 

