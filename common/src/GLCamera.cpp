#include "GLCamera.hpp"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

GLCamera::GLCamera(const char* name, QSize size)
{
    this->name = name;
    this->projection = glm::mat4(1.0f);
    this->view = glm::mat4(1.0f);
    this->fov = 45.0f;
    this->sensorDistance = 0.01f;
    this->focusDistance = 10000.0f;

    // Set the default camera angle
    this->radius  = 15.0f;
    this->azimuth = (4.0f*M_PI)/4.0f;
    this->zenith  = (2.0f*M_PI)/10.0f;

    // Surface is not tilted
    this->zenOffset = 0.0f;
    this->aziOffset = 0.0f;

    // Aim at the origin
    this->aimTarget = glm::vec3(0.0f,0.0f,0.0f);

    this->SetProjection(glm::perspective(
                            this->fov, 
                            float(size.width())/float(size.height()),
                            this->sensorDistance,
                            this->focusDistance) );

    updateView();
    this->updated = true;
}

GLCamera::~GLCamera()
{
}

void GLCamera::SetProjection(glm::mat4 projection)
{
    this->projection = projection;
}

void GLCamera::SetView(float radius, float azimuth, float zenith)
{
    this->radius = radius;
    this->azimuth = azimuth;
    this->zenith = zenith;
    updateView();
}

glm::mat4 GLCamera::Projection()
{
    return this->projection;
}

glm::mat4 GLCamera::View()
{
    return this->view;
}

glm::vec3 GLCamera::getCameraPosition()
{
    return glm::vec3(eyeX, eyeY, eyeZ);
}

void GLCamera::moveCamera(GLCamera::CamDirection direction)
{
    switch(direction)
    {
        case (CamDirection::Up):
            if(zenith >= ((24.0f*M_PI)/50.0f))
            {
                zenith = ((5.0f*M_PI)/10.0f);
            }
            else
            {
                zenith += (M_PI/50.0f);
            }
            break;
        case (CamDirection::Down):
            if(zenith <= ((2.0f*M_PI)/50.0f))
            {
                zenith = ((1.0*M_PI)/50.0f);
            }
            else
            {
                zenith -= (M_PI/50.0f);
            }
            break;
        case (CamDirection::Left):
            azimuth += (M_PI/50.0f);
            azimuth -= ((int)(azimuth/(2.0f*M_PI))*(2.0f*M_PI));
            break;
        case (CamDirection::Right):
            azimuth -= (M_PI/50.0f);
            azimuth -= ((int)(azimuth/(2.0f*M_PI))*(2.0f*M_PI));
            break;
        case (CamDirection::Forward):
            if(radius < 2.0f)
            {
                radius = 0.0f;
            }
            else
            {
                radius -= 2.0f;
            }
            break;
        case (CamDirection::Backward):
            if(radius > 178.0f)
            {
                radius = 180.0f;
            }
            else
            {
                radius += 2.0f;
            }
            break;
    }

    // Move the camera
    updateView();
}

void GLCamera::setCameraOffset(float zenith, float azimuth)
{
    this->zenOffset = zenith;
    this->aziOffset = azimuth;
    this->updateView();
}

void GLCamera::setAimTarget(glm::vec3 aimPos)
{
    this->aimTarget = aimPos;
}

void GLCamera::updateView()
{
    // Declare function variables
    //float eyeX, eyeY, eyeZ;

    // Perform fake object rotation (for the table)
    if(zenOffset != 0.0f || aziOffset != 0.0f )
    {

        // Calculate the first eye position

        // Calculate the eye position
        eyeX = radius*sin (zenith) * cos (azimuth);
        eyeZ = radius*sin (zenith) * sin (azimuth);
        eyeY = radius*cos (zenith);
    }
    else
    {
        // Calculate the eye position
        eyeX = radius * sin (zenith) * cos (azimuth);
        eyeZ = radius * sin (zenith) * sin (azimuth);
        eyeY = radius * cos (zenith);
    }

    // Compose a rotation matrix for the table
    glm::mat3 xMat = glm::mat3(1,0,0,
            0,cos(aziOffset),-sin(aziOffset),
            0,sin(aziOffset),cos(aziOffset));
    // glm::mat3 yMat = glm::mat3(cos(aziOffset),0,sin(aziOffset),
    //         0,1,0,
    //         -sin(aziOffset),0,cos(aziOffset));
    glm::mat3 zMat = glm::mat3(cos(zenOffset),-sin(zenOffset),0,
            sin(zenOffset),cos(zenOffset),0,
            0,0,1);
    glm::mat3 rotMat = zMat*xMat;
    this->homRotMat = glm::mat4(rotMat[0][0],rotMat[0][1],rotMat[0][2],0,
        rotMat[1][0],rotMat[1][1],rotMat[1][2],0,
        rotMat[2][0],rotMat[2][1],rotMat[2][2],0,
        0,0,0,1);

    // Set us up the view
    if (aimTarget != glm::vec3(0,0,0))
    {
        // Labyrinth
        this->view = (glm::lookAt(
                        glm::vec3(this->aimTarget.x, this->aimTarget.y+radius, this->aimTarget.z),  //eye pos
                        this->aimTarget,    //focus point
                        glm::vec3(1, 0, 0)))*homRotMat;  //up
    }
    else
    {
        // Airhockey
        this->view = (glm::lookAt(
                        glm::vec3(eyeX, eyeY, eyeZ),  //eye pos
                        glm::vec3(0,0,0),    //focus point
                        glm::vec3(0, 1, 0)))*homRotMat;  //up        
    }

}

bool GLCamera::Updated(bool update)
{
    this->updated = update;
    return this->updated;
}

glm::mat4 GLCamera::RotMat()
{
    return this->homRotMat;
}
