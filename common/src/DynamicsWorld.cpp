#include "DynamicsWorld.hpp"


DynamicsWorld::DynamicsWorld(const char* name) : GLNode(name)
{
    this->collisionConfiguration = std::unique_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());

    this->dispatcher = std::unique_ptr<btCollisionDispatcher>(new btCollisionDispatcher(this->collisionConfiguration.get()));
    this->overlappingPairCache = std::unique_ptr<btDbvtBroadphase>(new btDbvtBroadphase());
    this->solver = std::unique_ptr<btSequentialImpulseConstraintSolver>(new btSequentialImpulseConstraintSolver());
    this->world = std::unique_ptr<btDiscreteDynamicsWorld>(
    	               	     new btDiscreteDynamicsWorld( this->dispatcher.get(), 
    				         this->overlappingPairCache.get(),
    						 this->solver.get(),
    						 this->collisionConfiguration.get()));
    this->world->setGravity(btVector3(0,-10,0));
}

DynamicsWorld::~DynamicsWorld()
{
    for (int i=this->world->getNumCollisionObjects()-1; i>=0 ;i--)
    {
        btCollisionObject* obj = this->world->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
            this->world->removeCollisionObject( obj );
        }
    }
    solver.reset();
    overlappingPairCache.reset();
    dispatcher.reset();
    collisionConfiguration.reset();
}

void DynamicsWorld::AddPhysicsBody(std::shared_ptr<btRigidBody> body)
{
    this->world->addRigidBody(body.get());
}

void DynamicsWorld::AddConstraint(std::shared_ptr<btGeneric6DofConstraint> constraint)
{
    this->world->addConstraint(constraint.get());
}

void DynamicsWorld::RemovePhysicsBody(std::shared_ptr<btRigidBody> body)
{
    this->world->removeRigidBody(body.get());
}

void DynamicsWorld::RemoveConstraint(std::shared_ptr<btGeneric6DofConstraint> constraint)
{
    this->world->removeConstraint(constraint.get());
}

std::unique_ptr<btDiscreteDynamicsWorld> DynamicsWorld::GetWorld()
{
   return std::move(this->world);
}

void DynamicsWorld::SetWorld(std::unique_ptr<btDiscreteDynamicsWorld> world)
{
    this->world = std::move(world);
}

