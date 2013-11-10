#include "GLEmissive.hpp"


GLEmissive::GLEmissive(const char* name) : GLNode(name)
{
    this->lights.basic.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->lights.basic.diffuseIntensity = glm::vec4(0.2f, 0, 0, 0);
    this->lights.basic.ambientIntensity = glm::vec4(0.5f, 0, 0, 0);
    this->lights.basic.direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);

    /*
    this->lights.numPt = 1;
    this->lights.point.resize(1);
    this->lights.point[0].diffuseIntensity = glm::vec4(0.25f, 0, 0, 0);
    this->lights.point[0].color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
    this->lights.point[0].position = glm::vec4(3.0f, 1.0f, 2.0f, 1.0f);
    this->lights.point[0].linear = glm::vec4(0.1f, 0, 0, 0);
    this->lights.numSpt = 0;
    this->lights.point[1].diffuseIntensity = 0.25f;
    this->lights.point[1].color = glm::vec4(0.0f, 0.5f, 1.0f, 1.0f);
    this->lights.point[1].position = glm::vec4(7.0f, 1.0f, 2.0f, 1.0f);
    this->lights.point[1].attn.linear = 0.1f;
    numSpoint = 2;
    spot.resize(2);
    spot[0].diffuseIntensity = 0.9f;
    spot[0].color = glm::vec3(0.0f, 1.0f, 1.0f);
    spot[0].position = glm::vec3(-3.0f, 1.0f, 2.0f);
    spot[0].direction = glm::vec3(0.0f, 0.0f, 0.0f); 
    spot[0].attn.linear = 0.1f;
    spot[0].cutoff = 10.0f;

    spot[1].diffuseIntensity = 0.75f;
    spot[1].color = glm::vec3(0.0f, 0.5f, 1.0f);
    spot[1].position = glm::vec3(-7.0f, 1.0f, 2.0f);
    spot[1].attn.linear = 0.1f;
*/
    }


GLEmissive::~GLEmissive()
{
}


