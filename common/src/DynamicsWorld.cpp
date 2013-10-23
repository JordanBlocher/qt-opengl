#include "DynamicsWorld.hpp"


DynamicsWorld::DynamicsWorld(const char* name) : GLNode(name)
{
    this->collisionConfiguration = std::shared_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());
    this->dispatcher = std::shared_ptr<btCollisionDispatcher>(new btCollisionDispatcher(this->collisionConfiguration.get()));
    this->overlappingPairCache = std::shared_ptr<btDbvtBroadphase>(new btDbvtBroadphase());
    this->solver = std::shared_ptr<btSequentialImpulseConstraintSolver>(new btSequentialImpulseConstraintSolver());
    this->world = std::shared_ptr<btDiscreteDynamicsWorld>(
    	               	     new btDiscreteDynamicsWorld( this->dispatcher.get(), 
    				         this->overlappingPairCache.get(),
    						 this->solver.get(),
    						 this->collisionConfiguration.get()));
    this->world->setGravity(btVector3(0,-10,0));
}

DynamicsWorld::~DynamicsWorld()
{
}

void DynamicsWorld::AddPhysicsBody(std::shared_ptr<btRigidBody> body, std::shared_ptr<btGeneric6DofConstraint> constraint)
{
    this->world->addRigidBody(body.get());
    this->world->addConstraint(constraint.get());
}

std::shared_ptr<btDiscreteDynamicsWorld> DynamicsWorld::GetWorld()
{
   return this->world;
}

