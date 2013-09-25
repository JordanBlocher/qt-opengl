#ifndef GLBUFFEROBJECT_H
#define GLBUFFEROBJECT_H

#include <memory>

#include "GLNode.hpp"

class GLViewport;
class GLProgram;

class GLBufferObject : public GLNode
{

 public:
    GLBufferObject(const char*, GLsizeiptr, GLuint, GLuint, GLenum, GLenum);
    ~GLBufferObject();

    bool Status(GLenum, GLint);
    bool Status();
    void SetBlockIndex(GLuint);
    GLuint Buffer() const;
    GLenum Type() const;

 protected:
    GLuint buffer;
    GLuint block;
    GLenum type;

};

#endif 

