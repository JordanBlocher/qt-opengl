#include "PhysicsBody.hpp"

PhysicsBody::PhysicsBody(const char* name, float mass, float friction, float restitution, const glm::vec3 &size, const glm::vec3 &center, BODY type) : GLNode(name)
{
    this->mass = mass;
    this->friction = friction;
    this->restitution = restitution;

    switch(type)
    {
        case(CYLINDER):
            this->collisionShape = std::shared_ptr<btCylinderShape>( new btCylinderShape(btVector3(btScalar(size.x),btScalar(0.1),btScalar(size.x))) );
            this->SetMotionState(glm::vec3(0, 0, 0));
            this->SetConstraints(glm::vec3(0.0,0.01,0.0), glm::vec3(1, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
            break;
        default:
            std::cerr<< "[E] Rigid Body type not found."<<std::endl;
            return;
    }
}

// Constructor for custom convex triangle mesh
PhysicsBody::PhysicsBody(const char* name, const glm::vec3& size, const glm::vec3& center, std::vector<glm::vec4> vertices, const size_t numPoints) : GLNode(name)
{
    std::shared_ptr<btTriangleMesh> triMesh(new btTriangleMesh());

    glm::vec3 a0, a1, a2;
    // Create triangles
    for(size_t i=0; i<numPoints; i+=3)
    {
        a0 = glm::vec3( vertices[i+0].x, vertices[i+0].y, vertices[i+0].z );
        a1 = glm::vec3( vertices[i+1].x, vertices[i+1].y, vertices[i+1].z );
        a2 = glm::vec3( vertices[i+2].x, vertices[i+2].y, vertices[i+2].z );

        btVector3 v0(a0.x,a0.y,a0.z);
        btVector3 v1(a1.x,a1.y,a1.z);
        btVector3 v2(a2.x,a2.y,a2.z);

        triMesh->addTriangle(v0,v1,v2);
    }

    this->collisionShape = std::shared_ptr<btCollisionShape>(new btBvhTriangleMeshShape(triMesh.get(), true));

    //TODO: Hardcoded defaults?
    this->SetMotionState(glm::vec3(0, -0.1, 0));
}

// Set Initial Motion State
void PhysicsBody::SetMotionState(const glm::vec3 &transform)
{
    this->motionState = std::shared_ptr<btDefaultMotionState>( new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1), btVector3( transform.x, transform.y, transform.z))) );

    this->collisionShape->calculateLocalInertia( this->mass, this->inertia );

    btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo( this->mass, this->motionState.get(), this->collisionShape.get(), this->inertia );
    rigidBodyInfo.m_friction = friction;
    rigidBodyInfo.m_restitution = restitution;  

    this->rigidBody = std::shared_ptr<btRigidBody>(new btRigidBody( rigidBodyInfo ));

}

// Set Physics Constraints 6DOF
void PhysicsBody::SetConstraints(const glm::vec3 &origin, const glm::vec3 &linearLowerLimit, const glm::vec3 &linearUpperLimit, const glm::vec3 &angularLowerLimit, const glm::vec3 &angularUpperLimit)
{
    this->rigidBody->setActivationState(DISABLE_DEACTIVATION);
    this->rigidBody->setLinearFactor(btVector3(1,0,1));

    btTransform frame = btTransform::getIdentity();
    frame.setOrigin(btVector3(origin.x, origin.y, origin.z));

    this->planeConstraint = std::shared_ptr<btGeneric6DofConstraint>(new btGeneric6DofConstraint( *(this->rigidBody), frame, true ));

    // lowerlimit = upperlimit --> axis locked
    // lowerlimit < upperlimit --> motion limited between values
    // lowerlimit > upperlimit --> axis is free

    // lock the Y axis movement
    this->planeConstraint->setLinearLowerLimit( btVector3(linearLowerLimit.x, linearLowerLimit.y, linearLowerLimit.z) );
    this->planeConstraint->setLinearUpperLimit( btVector3(linearUpperLimit.x, linearUpperLimit.y, linearUpperLimit.z) );

    // lock the X, Z, rotations
    this->planeConstraint->setAngularLowerLimit( btVector3(angularLowerLimit.x, angularLowerLimit.y, angularLowerLimit.z) );
    this->planeConstraint->setAngularUpperLimit( btVector3(angularUpperLimit.x, angularUpperLimit.y, angularUpperLimit.z) );
}

PhysicsBody::~PhysicsBody()
{
}
