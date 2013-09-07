#ifndef GLCONTEXT_H
#define GLCONTEXT_H

#include <QGLWidget>
#include <QTimer>

#include <glm/glm.hpp>
#include <memory>
#include <chrono>

#include "GLStruct.hpp"

class GLNode;
class QGLApp;

class GLContext
{
 friend class QGLApp;

 public:
    GLContext();
    ~GLContext();
 
 protected:

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

    std::map<std::string, std::shared_ptr<GLNode>> QGLMap;

    Uniform GLUniform; //FIXME

};


#endif 

