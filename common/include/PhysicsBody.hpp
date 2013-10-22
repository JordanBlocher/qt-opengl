#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include "GLNode.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

enum BODY { CYLINDER, BOX, TRIMESH };

class PhysicsBody : public GLNode
{
 
 public:
    PhysicsBody(const char*, const glm::vec3&, const glm::vec3&, std::shared_ptr<glm::vec4>, const size_t, BODY);
    ~PhysicsBody();

protected:
    btScalar mass;
    btScalar friction;
    btScalar rest;

    std::shared_ptr<btRigidBody> body;
    std::shared_ptr<btCollisionShape> shape;
    std::shared_ptr<btCollisionShape> boundingBox;
    std::shared_ptr<btGeneric6DofConstraint> planeConstraint;

    std::shared_ptr<btDefaultMotionState> motionState;

};

#endif 

