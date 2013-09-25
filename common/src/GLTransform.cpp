#include "GLTransform.hpp"

#include <QGLWidget>

GLTransform::GLTransform(const char* name) : GLNode(name)
{
    this->matrix = glm::mat4(1.0);
}

GLTransform::GLTransform(const char* name, glm::mat4 transform) : GLNode(name)
{
    this->matrix = transform;
}

GLTransform::~GLTransform()
{
}

void GLTransform::Set(glm::mat4 transform)
{
    this->matrix = transform;
}

glm::mat4 GLTransform::Matrix()
{
    return this->matrix;
}

void GLTransform::Transform(glm::mat4 transform)
{
    this->matrix *= transform;
}
