#include "PhysicsModel.hpp"
#include "GLModel.hpp"

PhysicsModel::PhysicsModel(const char* name, const BODY type, const btVector3 scale, 
                const btQuaternion orientation, const btVector3 centroid, const float mass, const float friction, 
                const float restitution) : GLNode(name)
{
    // Initialize the collision shape
    initStandardShape(type, scale);

    // Initialize the transform
    initTransform(orientation, centroid);

    // Initialize the rigid body
    initRigidBody(mass,friction,restitution);
}

// Constructor for custom convex triangle mesh
PhysicsModel::PhysicsModel(const char* name, const std::shared_ptr<GLModel> model, const btVector3 scale, 
                            const btQuaternion orientation, const btVector3 centroid, const float mass, const float friction, 
                            const float restitution) : GLNode(name)
{
    // Initialize the collision shape
    initCustomShape(model, scale);

    // Initialize the transform
    initTransform(orientation, centroid);

    // Initialize the rigid body
    initRigidBody(mass,friction,restitution);
}

// Creates custom model
void PhysicsModel::initCustomShape(const std::shared_ptr<GLModel> model, const btVector3 scale)
{		

    // Allocate memory for the mesh triangles
    this->triangles = std::shared_ptr<btTriangleMesh>(new btTriangleMesh());
    this->triangles->preallocateIndices(model->Size());
    this->triangles->preallocateVertices(model->Size());   

    // Iterate over all of the meshes
    for(size_t meshIndex=0; meshIndex<model->Size(); meshIndex++)
    {

        // Iterate over all of the triangles of the mesh
        for(size_t i=0; i<model->Positions(meshIndex).size() - 2 ; i+=3)
        {
            // Get a triangle
            btVector3 a0( model->Positions(meshIndex).at(i+0).x*scale.getX(), 
                model->Positions(meshIndex).at(i+0).y*scale.getY(), 
                model->Positions(meshIndex).at(i+0).z*scale.getZ() );
            btVector3 a1( model->Positions(meshIndex).at(i+1).x*scale.getX(), 
                model->Positions(meshIndex).at(i+1).y*scale.getY(), 
                model->Positions(meshIndex).at(i+1).z*scale.getZ() );
            btVector3 a2( model->Positions(meshIndex).at(i+2).x*scale.getX(), 
                model->Positions(meshIndex).at(i+2).y*scale.getY(), 
                model->Positions(meshIndex).at(i+2).z*scale.getZ() );

            // Add it to the triangle set
            triangles->addTriangle(a0,a1,a2,false);
        }
    }

    // Build the shape
    this->collisionShape = std::shared_ptr<btCollisionShape>(new btBvhTriangleMeshShape(triangles.get(), true, true));

}

// Creates default-type model
void PhysicsModel::initStandardShape(const BODY type, const btVector3 scale)
{
    // Pick one of the standard shapes
    switch(type)
    {
        case(CYLINDER):
            this->collisionShape = std::shared_ptr<btCylinderShape>(new btCylinderShapeZ(scale));
            break;
        case(SPHERE):
            this->collisionShape = std::shared_ptr<btSphereShape>(new btSphereShape(scale.getX()));
            break;
        case(BOX):
            this->collisionShape = std::shared_ptr<btBoxShape>(new btBoxShape(scale));
            break;
        default:
            std::cerr<< "[E] PhysicsModel: Rigid Body type not found."<<std::endl;
            return;
    }
}

// Set us up the constraints
void PhysicsModel::initConstraints(const btVector3 origin, const btVector3 linearLowerLimit, const btVector3 linearUpperLimit, 
                                    const btVector3 angularLowerLimit, const btVector3 angularUpperLimit)
{
    // Create our constraint frame
    btTransform frame = btTransform::getIdentity();
    frame.setOrigin(origin);

    // Generic 6DOF (used as a plane) Constraint
    this->planeConstraint = std::shared_ptr<btGeneric6DofConstraint>(new btGeneric6DofConstraint( *(this->rigidBody), frame, true ));

    // Set those constraints
    this->planeConstraint->setLinearLowerLimit(linearLowerLimit);
    this->planeConstraint->setLinearUpperLimit(linearUpperLimit);
    this->planeConstraint->setAngularLowerLimit(angularLowerLimit);
    this->planeConstraint->setAngularUpperLimit(angularUpperLimit);
}

void PhysicsModel::initTransform(const btQuaternion orientation, const btVector3 centroid)
{
    // Set up the position transformation
    this->transform = btTransform::getIdentity();
    this->transform.setOrigin(centroid);
    this->transform.setRotation(orientation);

}
void PhysicsModel::initRigidBody(const float mass, const float friction, const float restitution)
{
    // Store the body parameters
    this->mass = mass;
    this->friction = friction;
    this->restitution = restitution;
    this->collisionShape->calculateLocalInertia( this->mass, this->inertia );
    this->motionState = std::make_shared<btDefaultMotionState>(this->transform);

    // Create a rigid body parameter object
    btRigidBody::btRigidBodyConstructionInfo rbInfo(this->mass, this->motionState.get(), this->collisionShape.get(), this->inertia);
    rbInfo.m_friction = this->friction;
    rbInfo.m_restitution = this->restitution;

    // Create the rigid body
    this->rigidBody = std::make_shared<btRigidBody>(rbInfo);
}


// Set Initial Motion State
void PhysicsModel::SetMotionState(const glm::vec3 &transform)
{
    // Set to default bullet Motion state (translation on z-axis)
    this->motionState = std::shared_ptr<btDefaultMotionState>( new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1), btVector3( transform.x, 0.0, transform.z))) );

    this->collisionShape->calculateLocalInertia( this->mass, this->inertia );

    btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo( this->mass, this->motionState.get(), this->collisionShape.get(), this->inertia );
    rigidBodyInfo.m_friction = friction;
    rigidBodyInfo.m_restitution = restitution;  

    this->rigidBody = std::shared_ptr<btRigidBody>(new btRigidBody( rigidBodyInfo ));

}

void PhysicsModel::Reset()
{
   this->SetMotionState(glm::vec3(0, 0, 0));
}

// Returns the transformation matrix of the body

//glm::mat4 PhysicsModel::GetRotation()
glm::mat4 PhysicsModel::GetTransform()
{
   this->rigidBody->getMotionState()->getWorldTransform(this->transform);
   btMatrix3x3 matrix = this->transform.getBasis();
   /*glm::mat4 ret(matrix[0][0], matrix[0][1], matrix[0][2], 0,
                 matrix[1][0], matrix[1][1], matrix[1][2], 0,
                 matrix[2][0], matrix[2][1], matrix[2][2], 0,
                 0, 0, 0, 1);
   */
   glm::mat4 ret(matrix[0][0], matrix[0][1], matrix[0][2], 0,
             matrix[1][0], matrix[1][1], matrix[1][2], 0,
             matrix[2][0], matrix[2][1], matrix[2][2], 0,
             this->transform.getOrigin().getX(), this->transform.getOrigin().getY(), this->transform.getOrigin().getZ(), 1);
                 return ret;
}

void PhysicsModel::SetTransform(btTransform newTransform)
{
    this->rigidBody->getMotionState()->setWorldTransform(newTransform);
}

// Returns the centroid
glm::mat4 PhysicsModel::GetTranslation()
{
   btVector3 origin = this->transform.getOrigin();
   glm::mat4 ret(1, 0, 0, (float)origin.getX(),
                 0, 1, 0, (float)origin.getY(),
                 0, 0, 1, (float)origin.getZ(),
                 0, 0, 0, 1);
   return ret;
}

std::shared_ptr<btRigidBody> PhysicsModel::GetRigidBody()
{
    return this->rigidBody;
}

std::shared_ptr<btGeneric6DofConstraint> PhysicsModel::GetConstraint()
{
    return this->planeConstraint;
}

PhysicsModel::~PhysicsModel()
{
}