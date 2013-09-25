#include "GLBufferObject.hpp"

#include <QGLWidget>
#include <QApplication>
#include <algorithm>
#include <climits>

//Uniform only
GLBufferObject::GLBufferObject(const char* name, GLsizeiptr dataSize, GLuint size, GLuint index, GLenum type, GLenum draw) : GLNode(name)
{
    this->type = type;
    this->block = UINT_MAX;
    glGenBuffers(1, &this->buffer);
    glBindBuffer(type, this->buffer);
    glBufferData(type, dataSize*size, NULL, draw);
}

GLBufferObject::~GLBufferObject()
{
}

bool GLBufferObject::Status(GLenum type, GLint size)
{
    glGetBufferParameteriv(type, GL_BUFFER_SIZE, &status);
    
    return (this->status == size && this->block != UINT_MAX);
}

bool GLBufferObject::Status()
{
    return this->block == UINT_MAX;
}

void GLBufferObject::SetBlockIndex(GLuint index)
{
    this->block = index;
}

GLuint GLBufferObject::Buffer() const
{
    return this->buffer;
}

GLenum GLBufferObject::Type() const
{
    return this->type;
}


