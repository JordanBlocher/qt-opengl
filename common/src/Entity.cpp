#include "Entity.hpp"

Entity::Entity(std::shared_ptr<GLModel> newGfx,std::shared_ptr<PhysicsBody> newPhys)
{
	this->gfxModel = newGfx;
	this->physModel = newPhys;
}


void Entity::setGraphicsModel(std::shared_ptr<GLModel> newModel)
{
	this->gfxModel = newModel;
}

void Entity::setPhysicsBody(std::shared_ptr<PhysicsBody> newModel)
{
	this->physModel = newModel;
}

std::shared_ptr<GLModel> Entity::getGraphicsModel()
{
	return gfxModel;
}

std::shared_ptr<PhysicsBody> Entity::getPhysicsBody()
{
	return physModel;
}