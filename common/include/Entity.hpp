#ifndef CS680_ENTITY_H
#define CS680_ENTITY_H

#include "GLModel.hpp"
#include "PhysicsBody.hpp"

class Entity : public GLNode
{
public:
	Entity(std::shared_ptr<GLModel>,std::shared_ptr<PhysicsBody>);
	void setGraphicsModel(std::shared_ptr<GLModel>);
	void setPhysicsBody(std::shared_ptr<PhysicsBody>);
	std::shared_ptr<GLModel> getGraphicsModel();
	std::shared_ptr<PhysicsBody> getPhysicsBody();
	
protected:
	std::shared_ptr<GLModel> gfxModel;
	std::shared_ptr<PhysicsBody> physModel;

};

#endif 

