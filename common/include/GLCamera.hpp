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
    void setCameraOffset(float zenith, float azimuth);
    void setAimTarget(glm::vec3 aimPos);

    bool Updated(bool = true);
    glm::mat4 Projection();
    glm::mat4 View();
    glm::mat4 RotMat();


protected:

    float fov;
    float sensorDistance;
    float focusDistance;
    float radius, azimuth, zenith;
    float eyeX,eyeY,eyeZ;
    bool updated;
    glm::vec3 aimTarget;

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 homRotMat;

    float zenOffset;
    float aziOffset;

    void updateView();
};

#endif 

