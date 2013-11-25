#include "GLEmissive.hpp"


GLEmissive::GLEmissive(const char* name) : GLNode(name)
{
    //Basic ambient/diffuse
    BaseLight base = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                      0.1f,
                      0.4f };
    DirectionalLight dir = {glm::vec4(0.0f, -1.0f, 0.0f, 1.0f),
                            base };
    this->lights.basic = dir; 

    //Pointlights
    this->lights.point.resize(1);
    BaseLight bpt0 = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                     0.1f,
                     0.1f};
    PointLight pt0 = {glm::vec4(0.0f, 20.0f, 0.0f, 1.0f),
                     bpt0 };
    this->lights.point[0] = pt0;


    //Spotlights
    this->lights.spot.resize(2);
    BaseLight bspt0 = {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                     0.9f,
                     0.9f};
    PointLight spt0 = {glm::vec4(0.0f, 5.0f, 0.0f, 1.0f),
                      bspt0};
    SpotLight sp0 = {glm::vec4(0.0, -1.0f, 0.0f, 1.0f),
                      spt0};
    this->lights.spot[0] = sp0;

    BaseLight bspt1 = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                     0.9f,
                     0.9f};
    PointLight spt1 = {glm::vec4(0.0f, 5.0f, 0.0f, 1.0f),
                      bspt1};
    SpotLight sp1 = {glm::vec4(0.0f, -1.0f, 0.0f, 1.0f),
                      spt1};
    this->lights.spot[1] = sp1;
}


GLEmissive::~GLEmissive()
{
}


