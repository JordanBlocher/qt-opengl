#include "PhysicsBody.hpp"

#include <QGLWidget>
#include<QApplication>


PhysicsBody::PhysicsBody(const char* name, const glm::vec3 &size, const glm::vec3 &center, std::shared_ptr<glm::vec4> points, const size_t numPoints) : GLNode(name)
{
}

PhysicsBody::PhysicsBody(const char* name, const glm::vec2 &size, const glm::vec3 &center, std::shared_ptr<glm::vec4> points, const size_t numPoints) : GLNode(name)
{
}

PhysicsBody::~PhysicsBody()
{
}

