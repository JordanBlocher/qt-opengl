#ifndef GLCAMERA_H
#define GLCAMERS_H

#include "GLNode.hpp"

#include <QWidget>

class GLCamera : public GLNode
{
 
 public:
    GLCamera(const char*, QSize);
    ~GLCamera();
    
    void SetProjection(glm::mat4);
    void SetView(glm::mat4);

    glm::mat4 Projection();
    glm::mat4 View();

protected:

    float fov;
    float sensorDistance;
    float focusDistance;

    glm::mat4 projection;
    glm::mat4 view;
};

#endif 

