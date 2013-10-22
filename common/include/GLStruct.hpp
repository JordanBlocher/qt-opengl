#ifndef GLSTRUCT_H
#define GLSTRUCT_H

#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <btBulletDynamicsCommon.h>

struct Material
{
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    glm::vec3 transparent;
    float transparency;
    bool texture;
};

struct Dynamics
{
    std::shared_ptr<btDiscreteDynamicsWorld> world;
    std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
    std::shared_ptr<btCollisionDispatcher> dispatcher;
    std::shared_ptr<btBroadphaseInterface> overlappingPairCache;
    std::shared_ptr<btSequentialImpulseConstraintSolver> solver;

};

#endif

