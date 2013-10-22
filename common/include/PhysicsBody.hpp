#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include "GLNode.hpp"

#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

class PhysicsNode : public GLnode
{
 
 public:
    PhysicsBody(const char*, const &glm::vec3, const & glm::vec3, std::shared_ptr<glm::vec4>, const size_t);
    PhysicsBody(const char*, const &glm::vec2, const & glm::vec3, std::shared_ptr<glm::vec4>, const size_t);
    ~PhysicsBody();

 
protected:
    btScalar mass;
    btScalar friction;
    btScalar rest;

    std::shared_ptr<btRigidBody> body;
    std::shared_ptr<btCollisionShape> shape;
    std::shared_prt<btCollisionShape> boundingBox;
    std::shared_ptr<btGeneric6DofConstraint> planeConstraint;

};

#endif 

