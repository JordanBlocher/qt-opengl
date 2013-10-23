#ifndef CS680_ENTITY_H
#define CS680_ENTITY_H

#include "GLModel.hpp"
#include "PhysicsModel.hpp"

class Entity : public GLNode
{
public:

	Entity(std::shared_ptr<GLModel>,std::shared_ptr<PhysicsModel>);
	void setGraphicsModel(std::shared_ptr<GLModel>);
	void setPhysicsModel(std::shared_ptr<PhysicsModel>);
	std::shared_ptr<GLModel> getGraphicsModel();
	std::shared_ptr<PhysicsModel> getPhysicsModel();
	
protected:
	std::shared_ptr<GLModel> gfxModel;
	std::shared_ptr<PhysicsModel> physModel;
    
};

#endif 

