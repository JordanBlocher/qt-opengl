#include <GL/glew.h>
#include <QGLWidget>
#include <iostream>
#include <cerrno>
#include<QApplication>
#include <algorithm>

#include "GLBufferObject.hpp"

//Uniform only
GLBufferObject::GLBufferObject(const char* name, GLsizeiptr dataSize, GLuint size, GLuint index, GLenum type) : GLNode(name)
{
    this->type = type;
    this->block = -1;
    glGenBuffers(1, &this->buffer);
    glBindBuffer(type, this->buffer);
    glBufferData(type, dataSize*size, NULL, GL_STREAM_DRAW);
}

GLBufferObject::~GLBufferObject()
{
   /* for_each( this->uniforms->begin(), this->uniforms->end(), &deletePtr<Uniform> );
    this->uniforms->clear();*/
}

bool GLBufferObject::Status(GLenum type, GLint size)
{
    glGetBufferParameteriv(type, GL_BUFFER_SIZE, &status);
    
    return (this->status == size && this->block != -1);
}

bool GLBufferObject::Status()
{
    return this->block == -1;
}

void GLBufferObject::SetBlockIndex(GLuint index)
{
    this->block = index;
}

GLuint GLBufferObject::getBuffer() const
{
    return this->buffer;
}

GLenum GLBufferObject::getType() const
{
    return this->type;
}

Uniform GLBufferObject::getUniform(const char* name)
{
    return this->uniforms->find(name)->second;
}

void GLBufferObject::AddUniform(std::pair<std::string, Uniform> pair)
{
    if( this->uniforms == NULL )
    {
        this->uniforms = UniformPtr(new UniformMap);
    }
    this->uniforms->insert(pair);
}

