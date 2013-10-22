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

    void AddPhysicsBody(std::shared_ptr<btRigidBody>, std::shared_ptr<btGeneric6DofConstraint>);

protected:

    std::shared_ptr<btDiscreteDynamicsWorld> world;
    std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
    std::shared_ptr<btCollisionDispatcher> dispatcher;
    std::shared_ptr<btBroadphaseInterface> overlappingPairCache;
    std::shared_ptr<btSequentialImpulseConstraintSolver> solver;

};

#endif 

