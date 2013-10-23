#include "Entity.hpp"

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

std::shared_ptr<GLModel> Entity::getGraphicsModel()
{
	return gfxModel;
}

std::shared_ptr<PhysicsModel> Entity::getPhysicsModel()
{
	return physModel;
}

