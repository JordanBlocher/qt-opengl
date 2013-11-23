#ifndef CS680_ENTITY_H
#define CS680_ENTITY_H

#include "GLNode.hpp"

class PhysicsModel;
class GLModel;
class DynamicsWorld;

class Entity : public GLNode
{
 public:
    enum BODY {STATIC, DYNAMIC, DYNAMIC_NO_G, SPHERE};

	Entity(float, float, float, float, btVector3);
	Entity(std::shared_ptr<GLModel>,std::shared_ptr<PhysicsModel>);
	void setGraphicsModel(std::shared_ptr<GLModel>);
	void setPhysicsModel(std::shared_ptr<PhysicsModel>);
	std::shared_ptr<GLModel> GetGraphicsModel();
	std::shared_ptr<PhysicsModel> GetPhysicsModel();

    void Create(const char*, const char* phyfile = NULL, BODY type = DYNAMIC); 
    void Constrain(BODY);
	
protected:
	std::shared_ptr<GLModel> gfxModel;
	std::shared_ptr<PhysicsModel> physModel;

    // Convenience
    float scale, mass, restitution, friction;
    btVector3 centroid;
    
};

#endif 

