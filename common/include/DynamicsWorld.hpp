#ifndef DYNAMICSWORLD_H
#define DYNAMICSWORLD_H

#include "GLNode.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

class DynamicsWorld : public GLNode
{
 
 public:
    DynamicsWorld(const char*);
    ~DynamicsWorld();

    void AddPhysicsBody(std::shared_ptr<btRigidBody>);
    void AddConstraint(std::shared_ptr<btGeneric6DofConstraint>);
    std::unique_ptr<btDiscreteDynamicsWorld> GetWorld();
    void SetWorld(std::unique_ptr<btDiscreteDynamicsWorld> world);

protected:

    std::unique_ptr<btDiscreteDynamicsWorld> world;
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btBroadphaseInterface> overlappingPairCache;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;

};

#endif 

