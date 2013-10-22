#include "PhysicsBody.hpp"


PhysicsBody::PhysicsBody(const char* name, const glm::vec3 &size, const glm::vec3 &center, std::shared_ptr<glm::vec4> points, const size_t numPoints, BODY type) : GLNode(name)
{
    this->body = std::shared_ptr<btRigidBody>(new btRigidBody());
    this->shape = std::shared_ptr<btCollisionShape>(new btCollisionShape());
    this->boundingBox = std::shared_ptr<btCollisionShape>(new btCollisionShape());
    this->planeConstraint = std::shared_ptr<btGeneric6DofConstraint>(new btGeneric6DofConstraint());

    this->motionState = std::shared_ptr<btDefaultMotionState>(new btDefaultMotionState());

}

void PhysicsBody::CreateCylinder(const glm::vec3 &size, const glm::vec3 &center, std::shared_ptr<glm::vec4> points, const size_t numPoints)
{
    this->shape = std::shared_ptr<btCylinderShape>(new btCylinderShape(btVector3(btScalar(puckSize.x),btScalar(0.1),btScalar(puckSize.x)));

    this->motionState = std::shared_ptr<btDefaultMotionState>(new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1), btVector3( 0,0.0,0)));

   btVector3 puckInertia(0,0,0);
   puckShape->calculateLocalInertia( puck_mass, puckInertia);

   btRigidBody::btRigidBodyConstructionInfo puckRigidBodyCI( puck_mass, puckMotionState, puckShape, puckInertia);
   puckRigidBodyCI.m_friction = puck_friction;        //this is the friction of its surfaces
   puckRigidBodyCI.m_restitution = puck_restitution;  //this is the "bounciness"

   puckRigidBody = new btRigidBody( puckRigidBodyCI );
   dynamicsWorld->addRigidBody( puckRigidBody );

   puckRigidBody->setActivationState(DISABLE_DEACTIVATION);
   puckRigidBody->setLinearFactor(btVector3(1,0,1));

   {
      btTransform frameInB = btTransform::getIdentity();
      frameInB.setOrigin(btVector3(0.0,0.01,0.0));

      puckXZplaneConstraint = new btGeneric6DofConstraint( *puckRigidBody,frameInB, true );

      // lowerlimit = upperlimit --> axis locked
      // lowerlimit < upperlimit --> motion limited between values
      // lowerlimit > upperlimit --> axis is free

      // lock the Y axis movement
      puckXZplaneConstraint->setLinearLowerLimit( btVector3(1,0,1));
      puckXZplaneConstraint->setLinearUpperLimit( btVector3(0,0,0));

      // lock the X, Z, rotations
      puckXZplaneConstraint->setAngularLowerLimit(btVector3(0,1,0));
      puckXZplaneConstraint->setAngularUpperLimit(btVector3(0,0,0));

      dynamicsWorld->addConstraint(puckXZplaneConstraint);
   }
}

PhysicsBody::~PhysicsBody()
{
}


