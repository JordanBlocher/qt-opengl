#include <GL/glew.h>
#include <QGLWidget>
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <stdio.h>

#include "GLNode.hpp"

using namespace std;

GLNode::GLNode()
{
}

GLNode::~GLNode()
{
}

GLuint GLNode::getId()
{
    return this->id;
}

bool GLNode::Status()
{
    return false;
}

void GLNode::Add(GLNode node)
{
}

void GLNode::Remove(GLNode node)
{
}

