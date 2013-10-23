#include "GLNode.hpp"
#include "GLProgram.hpp"

#include <QGLWidget>
#include<QApplication>

typedef std::pair<std::string, std::shared_ptr<GLNode>> QGLPair;

GLNode::GLNode()
{
}

GLNode::GLNode(const std::shared_ptr<GLNode> node)
{
    this->id = node->id;
    this->name = node->name;
}


GLNode::GLNode(const char* name)
{
    this->name = name;
}

GLNode::GLNode(const char* name, GLenum type)
{
    this->type = type;
}

GLNode::~GLNode()
{
}

GLuint GLNode::getId() const
{
    return this->id;
}

std::string GLNode::getName() const
{
    return this->name;
}

bool GLNode::Status()
{
    return false;
}


