#ifndef PHYSICSMODEL_H
#define PHYSICSMODEL_H

#include "GLNode.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

class GLModel;

class PhysicsModel : public GLNode
{
 
 public:
    enum BODY { CYLINDER, BOX, SPHERE };
    // Default shape constructor
    PhysicsModel(const char* name, const BODY type, const btVector3 scale, 
                const btQuaternion orientation, const btVector3 centroid, const float mass, const float friction, 
                const float restitution);

    // Custom shape constructor
    PhysicsModel(const char* name, const std::shared_ptr<GLModel> model, const btVector3 scale, 
                const btQuaternion orientation, const btVector3 centroid, const float mass, const float friction, 
                const float restitution);
    ~PhysicsModel();

    void SetMotionState(const glm::vec3&);
    void initConstraints(const btVector3 origin, const btVector3 linearLowerLimit, const btVector3 linearUpperLimit, 
                                    const btVector3 angularLowerLimit, const btVector3 angularUpperLimit);
    //glm::mat4 GetRotation();
    glm::mat4 GetTransform();
    void SetTransform(btTransform newTranform);
    void Reset();
    std::shared_ptr<btRigidBody> GetRigidBody();
    std::shared_ptr<btGeneric6DofConstraint> GetConstraint();

protected:
    void initCustomShape(const std::shared_ptr<GLModel> model, const btVector3 scale);
    void initStandardShape(const BODY type, const btVector3 scale);

    void initTransform(const btQuaternion orientation, const btVector3 centroid);
    void initRigidBody(const float mass, const float friction, const float restitution);

    btScalar mass;
    btScalar friction;
    btScalar restitution; // bouncy coeff
    btVector3 inertia;

    std::shared_ptr<btRigidBody> rigidBody;
    std::shared_ptr<btCollisionShape> collisionShape;
    std::shared_ptr<btCollisionShape> boundingBox;
    std::shared_ptr<btGeneric6DofConstraint> planeConstraint;

    std::shared_ptr<btDefaultMotionState> motionState;
   
    std::shared_ptr<btTriangleMesh> triangles;

    btTransform transform;

    glm::vec3 pos_towards;
    glm::vec3 pos_away;

};
#endif 

