#include "GLEmissive.hpp"


GLEmissive::GLEmissive(const char* name) : GLNode(name)
{
    //Basic ambient/diffuse
    BaseLight base = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                      0.2f,
                      0.6f };
    DirectionalLight dir = {glm::vec4(0.0f, -1.0f, 0.0f, 1.0f),
                            base };
    this->lights.basic = dir; 

    //Pointlights
    this->lights.point.resize(1);
    BaseLight bpt0 = {glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
                     0.5f,
                     0.95f};
    PointLight pt0 = {glm::vec4(3.0f, 1.0f, 2.0f, 1.0f),
                     bpt0 };
    this->lights.point[0] = pt0;

    //Spotlights
    this->lights.spot.resize(1);
    BaseLight bspt0 = {glm::vec4(1.0f, 0.5f, 1.0f, 1.0f),
                     0.5f,
                     0.2f};
    PointLight pspt0 = {glm::vec4(1.0f, 2.0f, 0.0f, 1.0f),
                     bspt0};
    SpotLight spt0 = {glm::vec4(-0.5, -1.0f, 0.0f, 1.0f),
                     pspt0};

    this->lights.spot[0] = spt0;

}


GLEmissive::~GLEmissive()
{
}


