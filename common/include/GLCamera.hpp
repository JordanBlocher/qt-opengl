#ifndef GLCAMERA_H
#define GLCAMERS_H

#include "GLNode.hpp"

#include <QWidget>

#include <glm/glm.hpp>

class GLCamera : public GLNode
{
 
public:
    enum class CamDirection {Up, Down, Left, Right, Forward, Backward};
    GLCamera(const char*, QSize);
    ~GLCamera();
    
    void SetProjection(glm::mat4);
    void SetView(float radius, float azimuth, float zenith);
    void moveCamera(CamDirection direction);
    glm::vec3 getCameraPosition();

    glm::mat4 Projection();
    glm::mat4 View();


protected:

    float fov;
    float sensorDistance;
    float focusDistance;
    float radius, azimuth, zenith;
    float eyeX,eyeY,eyeZ;

    glm::mat4 projection;
    glm::mat4 view;

    void updateView();
};

#endif 

