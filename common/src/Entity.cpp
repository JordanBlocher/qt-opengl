#include "Entity.hpp"

#include "GLModel.hpp"
#include "PhysicsModel.hpp"
#include "DynamicsWorld.hpp"

#include <glm/gtc/matrix_transform.hpp>

Entity::Entity(float s, float m, float r, float f, btVector3 c)
    : scale(s), mass(m), restitution(r), friction(f), centroid(c)
{
}

Entity::Entity(std::shared_ptr<GLModel> newGfx,std::shared_ptr<PhysicsModel> newPhys)
{
	this->gfxModel = newGfx;
	this->physModel = newPhys;
}

void Entity::setGraphicsModel(std::shared_ptr<GLModel> newModel)
{
	this->gfxModel = newModel;
}

void Entity::setPhysicsModel(std::shared_ptr<PhysicsModel> newModel)
{
	this->physModel = newModel;
}

std::shared_ptr<GLModel> Entity::GetGraphicsModel()
{
	return gfxModel;
}

std::shared_ptr<PhysicsModel> Entity::GetPhysicsModel()
{
	return physModel;
}

// Use this function to create standard physics objects, requires models with vertices, normals and uv coordinates.
void Entity::Create(const char* gfxfile, const char* physfile, BODY type)
{
    btQuaternion orientation = btQuaternion(0, 0, 0, 1);
    
    this->gfxModel = std::shared_ptr<GLModel>(new GLModel(gfxfile, gfxfile, 3));
    this->gfxModel->CreateVAO();
    this->gfxModel->setMatrix(glm::scale(this->gfxModel->Matrix(), glm::vec3(this->scale))); 

    if(type == STATIC)
    {
      if(physfile != NULL)
        {
            std::shared_ptr<GLModel> constraint(new GLModel(physfile, physfile, 3));
            constraint->LoadVertexData();
            this->physModel = std::shared_ptr<PhysicsModel>(new PhysicsModel(gfxfile, constraint, btVector3(scale, scale, scale), orientation, this->centroid, 0.0f, this->friction, this->restitution, PhysicsModel::COLLISION::STATIC));
        }
        else
            this->physModel = std::shared_ptr<PhysicsModel>(new PhysicsModel(gfxfile, this->gfxModel, btVector3(scale, scale, scale), orientation, this->centroid, 0.0f, this->friction, this->restitution, PhysicsModel::COLLISION::STATIC));
    }
    else if(type == DYNAMIC || type == DYNAMIC_NO_G)
    {
        if(physfile != NULL)
        {
            std::shared_ptr<GLModel> constraint(new GLModel(physfile, physfile, 3));
            constraint->LoadVertexData();
            this->physModel = std::shared_ptr<PhysicsModel>(new PhysicsModel(gfxfile, constraint, btVector3(scale, scale, scale), orientation, this->centroid, this->mass, this->friction, this->restitution, PhysicsModel::COLLISION::DYNAMIC));
        }
        else
            this->physModel = std::shared_ptr<PhysicsModel>(new PhysicsModel(gfxfile, this->gfxModel, btVector3(scale, scale, scale), orientation, this->centroid, this->mass, this->friction, this->restitution, PhysicsModel::COLLISION::DYNAMIC));
    }
    else if(type == SPHERE)
    {
       this->physModel = std::shared_ptr<PhysicsModel>(new PhysicsModel(gfxfile, PhysicsModel::BODY::SPHERE, btVector3(scale, scale, scale), orientation, this->centroid, this->mass, this->friction, this->restitution));

    }

}

void Entity::Constrain(BODY type)
{
    btVector3 ones = btVector3(1.0f, 1.0f, 1.0f);
    btVector3 zeros = btVector3(0.0f, 0.0f, 0.0f);

    if(type == DYNAMIC)
    {
        this->physModel->initConstraints(zeros, ones, zeros, ones, zeros);
        this->physModel->GetRigidBody()->setDamping(0.5f, 0.5f);
        this->physModel->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    }
    else if(type == DYNAMIC_NO_G)
    {
        this->physModel->initConstraints(zeros, ones, zeros, btVector3(0.0f,1.0f,0.0f), zeros);
        this->physModel->GetRigidBody()->setDamping(0.5f, 0.5f);

        // Set Restrictive Dynamics Constraints
        this->physModel->GetRigidBody()->setLinearFactor(btVector3(1,0,1));
        this->physModel->GetRigidBody()->setAngularFactor(btVector3(0,1,0));
        this->physModel->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    }

}

