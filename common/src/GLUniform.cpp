#include "GLUniform.hpp"
#include "GLBufferObject.hpp"

GLUniform::GLUniform() {}

GLUniform::GLUniform(const char* n, GLuint s, GLuint i, GLuint o) : GLNode(n), size(s), index(i), offset(o)
{
}

GLUniform::GLUniform(const char* name, std::vector<std::string> attributes, GLsizeiptr size, GLuint offset, GLuint draw)
{
    this->name = name;
    this->attributes = attributes;
    GLBufferObject uniform(name, size, attributes.size(), offset, GL_UNIFORM_BUFFER, draw);
}

GLUniform::~GLUniform()
{
}

void GLUniform::setBuffer(GLuint buffer)
{
    this->bufferId = buffer;
}

GLuint GLUniform::Size()
{
    return this->size;
}

GLuint GLUniform::Index()
{
    return this->index;
}

GLuint GLUniform::Offset()
{
    return this->offset;
}

GLuint GLUniform::Buffer()
{
    return this->bufferId;
}
