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
    this->radius  = 25.0f;
    this->azimuth = (6.0f*M_PI)/4.0f;
    this->zenith  = (2.0f*M_PI)/10.0f;


    this->SetProjection(glm::perspective(
                            this->fov, 
                            float(size.width())/float(size.height()),
                            this->sensorDistance,
                            this->focusDistance) );

    updateView();
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

void GLCamera::updateView()
{
    // Declare function variables
    float eyeX, eyeY, eyeZ;
    float upX, upY, upZ;
    float magnitude;

    // Calculate the eye position
    eyeX = radius * sin (zenith) * cos (azimuth);
    eyeZ = radius * sin (zenith) * sin (azimuth);
    eyeY = radius * cos (zenith);

    // Do the cross product manually for some reason
    upX = (-eyeX*eyeZ);
    upY = (eyeZ*eyeY);
    upZ = ((-eyeX*eyeX) - (eyeY*eyeY));
    magnitude = sqrt((upX*upX) + (upY*upY) + (upZ*upZ));

    // Make that into a unit vector
    upX /= magnitude;
    upY /= magnitude;
    upZ /= magnitude;

    // If upside down, flip
    if (upZ < 0)
    {
        upX = -upX;
        upY = -upY;
        upZ = -upZ;   
    }

    // Set us up the view
    this->view = (glm::lookAt(
                    glm::vec3(eyeX, eyeY, eyeZ),  //eye pos
                    glm::vec3(0.0, 0.0, 0.0),    //focus point
                    glm::vec3(0, 1, 0)) );  //up
}
