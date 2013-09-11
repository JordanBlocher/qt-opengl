#ifndef GLCONTEXT_H
#define GLCONTEXT_H

#include <QGLWidget>
#include <QTimer>

#include <glm/glm.hpp>
#include <memory>
#include <chrono>

#include "GLStruct.hpp"

class GLNode;
class QGLView;

typedef std::map<std::string,std::shared_ptr<GLNode>> GLMap; 
typedef std::shared_ptr<GLMap> GLMapPtr; 

class GLContext
{
 
 public:
    GLContext(GLMapPtr m = NULL);
    ~GLContext();

 protected:

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

//    static std::map<std::string, std::shared_ptr<GLNode>> QGLMap;
    GLMapPtr QGLMap;
};


#endif 

