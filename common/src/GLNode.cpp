#include <GL/glew.h>
#include <QGLWidget>
#include <iostream>
#include <cerrno>
#include<QApplication>

#include "GLNode.hpp"
#include "GLProgram.hpp"
#include "GLContext.hpp"

typedef std::pair<std::string, std::shared_ptr<GLNode>> QGLPair;

GLNode::GLNode()
{
}

GLNode::GLNode(const GLNode& node)
{
    this->id = node.id;
    this->name = node.name;
}


GLNode::GLNode(const char* name)
{
    this->name = name;
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

bool GLNode::Add(std::shared_ptr<GLNode> node = NULL)
{
    return false;
}

bool GLNode::Remove(const char* name = NULL)
{
    return false;
}

