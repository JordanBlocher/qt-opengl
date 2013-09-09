#include <GL/glew.h>
#include <memory>
#include <iostream>

#include "GLContext.hpp"

typedef std::pair<std::string, std::shared_ptr<GLNode>> GLPair;

GLContext::GLContext(GLMapPtr m)
{
    if ( m == NULL )
        QGLMap = GLMapPtr(new GLMap);
    else
        QGLMap = m;
}

GLContext::~GLContext()
{
}

//std::chrono::time_point<std::chrono::high_resolution_clock> GLContext::start_time;

//std::map<std::string, std::shared_ptr<GLNode>> GLContext::QGLMap;



