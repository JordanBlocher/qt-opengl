#ifndef PHYSICSMODEL_H
#define PHYSICSMODEL_H

#include "GLNode.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>


class PhysicsModel : public GLNode
{
 
 public:
    enum BODY { CYLINDER, BOX, SPHERE };
    PhysicsModel(const char*, float, float, float, const glm::vec3&, const glm::vec3&, BODY);
    PhysicsModel(const char*, const std::vector<glm::vec3>&, size_t);
    ~PhysicsModel();

    void SetMotionState(const glm::vec3&);
    void SetConstraints(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&);
    //glm::mat4 GetRotation();
    glm::mat4 GetTransform();
    void SetTransform(glm::vec4, glm::vec3);
    glm::mat4 GetTranslation();
    void SetTransform(btTransform newTranform);
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
   
    std::shared_ptr<std::vector<btTriangleMesh>> triangles;
    btTransform transform;

    glm::vec3 pos_towards;
    glm::vec3 pos_away;

};
#endif 

