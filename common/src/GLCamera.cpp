#include "GLCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

GLCamera::GLCamera(const char* name, QSize size)
{
    this->name = name;
    this->projection = glm::mat4(1.0f);
    this->view = glm::mat4(1.0f);
    this->fov = 45.0f;
    this->sensorDistance = 0.01f;
    this->focusDistance = 100.0f;

    this->SetProjection(glm::perspective(
                            this->fov, 
                            float(size.width())/float(size.height()),
                            this->sensorDistance,
                            this->focusDistance) );

    this->SetView(glm::lookAt(
                    glm::vec3(0.0, 8.0, -16.0),  //eye pos
                    glm::vec3(0.0, 0.0, 0.0),    //focus point
                    glm::vec3(0.0, 1.0, 0.0)) );  //up
}

GLCamera::~GLCamera()
{
}

void GLCamera::SetProjection(glm::mat4 projection)
{
    this->projection = projection;
}

void GLCamera::SetView(glm::mat4 view)
{
    this->view = view;
}

glm::mat4 GLCamera::Projection()
{
    return this->projection;
}

glm::mat4 GLCamera::View()
{
    return this->view;
}

