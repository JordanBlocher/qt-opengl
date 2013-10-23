#ifndef PHYSICSMODEL_H
#define PHYSICSMODEL_H

#include "GLNode.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>


class PhysicsModel : public GLNode
{
 
 public:
    PhysicsModel(const char*, float, float, float, const glm::vec3&, const glm::vec3&);
    PhysicsModel(const char*, const std::vector<glm::vec3>&, size_t);
    ~PhysicsModel();

    void SetMotionState(const glm::vec3&);
    void SetConstraints(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&);
    glm::mat4 GetRotation();
    void SetTransform(glm::vec4, glm::vec3);
    glm::mat4 GetTranslation();
    void Reset();
    std::shared_ptr<btRigidBody> GetRigidBody();
    std::shared_ptr<btGeneric6DofConstraint> GetConstraint();

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
   
    std::shared_ptr<std::vector<std::shared_ptr<btTriangleMesh>>> triangles;
    btTransform transform;

    glm::vec3 pos_towards;
    glm::vec3 pos_away;

};
#endif 

