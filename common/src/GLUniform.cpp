#include "GLUniform.hpp"

GLUniform::GLUniform() {}

GLUniform::GLUniform(const char* n, GLuint s, GLuint i, GLuint o) : GLNode(n), size(s), index(i), offset(o)
{
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
