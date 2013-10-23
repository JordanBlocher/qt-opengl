#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include "GLNode.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

enum BODY { CYLINDER, BOX, SPHERE };

class PhysicsBody : public GLNode
{
 
 public:
    PhysicsBody(const char*, float, float, float, const glm::vec3&, const glm::vec3&, BODY);
    PhysicsBody(const char*, const glm::vec3&, const glm::vec3&, std::vector<glm::vec4>, const size_t);
    ~PhysicsBody();

    void SetMotionState(const glm::vec3&);
    void SetConstraints(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&);
    glm::mat4 GetTransform();
    glm::vec3 GetOrigin();
    void Reset();

protected:
    btScalar mass;
    btScalar friction;
    btScalar restitution; // bouncy coeff
    btVector3 inertia;

    std::shared_ptr<btRigidBody> rigidBody;
    std::shared_ptr<btCollisionShape> collisionShape;
    std::shared_ptr<btCollisionShape> boundingBox;
    std::shared_ptr<btGeneric6DofConstraint> planeConstraint;

    std::shared_ptr<btDefaultMotionState> motionState;

    btTransform transform;

    glm::vec3 pos_towards;
    glm::vec3 pos_away;

};

#endif 

