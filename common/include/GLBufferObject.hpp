#ifndef GLBUFFEROBJECT_H
#define GLBUFFEROBJECT_H

#include <memory>
#include <vector>

#include "GLNode.hpp"

class GLViewport;
class GLProgram;

class GLBufferObject : public GLNode
{

 public:
    GLBufferObject(const char*, GLsizeiptr, GLuint, GLuint, GLenum, GLenum);

    template< class T >
    GLBufferObject(const char*, GLsizeiptr, GLuint&, std::shared_ptr<std::vector<T>>);
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

// Default VBO
template < class T >
GLBufferObject::GLBufferObject(const char* name, GLsizeiptr dataSize, GLuint &buffer, std::shared_ptr<std::vector<T>> vbo) : GLNode(name)
{
    this->type = GL_ARRAY_BUFFER;
    this->block = UINT_MAX;
    glGenBuffers(1, &this->buffer);
    glBindBuffer(this->type, this->buffer);
    glBufferData(this->type, dataSize*vbo->size(), vbo->data(), GL_STATIC_DRAW);
    buffer = this->buffer;
}


#endif 

