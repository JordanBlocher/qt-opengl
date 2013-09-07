#include <GL/glew.h>
#include <QGLWidget>
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>

#include<QApplication>

#include "GLNode.hpp"
#include "GLProgram.hpp"
#include "GLContext.hpp"

typedef std::pair<std::string, std::shared_ptr<GLNode>> QGLPair;

using namespace std;

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
    return( !GLContext::QGLMap.empty() ); 
}

bool GLNode::Add(std::shared_ptr<GLNode> node)
{
    if( GLContext::QGLMap.count(node->name) == 0 )
    {
        GLContext::QGLMap.insert(QGLPair(node->name, node));
        return true;
    }
    return false;
}

bool GLNode::Remove(const char* name)
{
    if( GLContext::QGLMap.count(name) == 1 )
    {
        GLContext::QGLMap.erase(name);
        return true;
    }
    return false;
}

