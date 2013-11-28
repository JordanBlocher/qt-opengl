#include "GLEmissive.hpp"


GLEmissive::GLEmissive(const char* name) : GLNode(name)
{
    //Basic ambient/diffuse
    BaseLight base = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                      0.1f,
                      0.7f };
    DirectionalLight dir = {glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                            base };
    this->lights.basic = dir; 

    //Pointlights
    this->lights.point.resize(1);
    BaseLight bpt0 = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                     0.1f,
                     0.7f};
    PointLight pt0 = {glm::vec4(0.0f, 50.0f, 0.0f, 1.0f),
                     bpt0 };
    this->lights.point[0] = pt0;

    //Spotlights
    this->lights.spot.resize(6);
    BaseLight bspt0 = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                     0.1f,
                     0.1f};
    PointLight spt0 = {glm::vec4(50.0f, -50.0f, 0.0f, 1.0f),
                      bspt0};
    SpotLight sp0 = {glm::vec4(-1.0, 1.0f, 0.0f, 1.0f),
                      spt0};
    this->lights.spot[0] = sp0;

    BaseLight bspt1 = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                     0.1f,
                     0.1f};
    PointLight spt1 = {glm::vec4(0.0f, -50.0f, 50.0f, 1.0f),
                      bspt1};
    SpotLight sp1 = {glm::vec4(0.0f, 1.0f, -1.0f, 1.0f),
                      spt1};
    this->lights.spot[1] = sp1;

    BaseLight bspt2 = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                     0.1f,
                     0.1f};
    PointLight spt2 = {glm::vec4(0.0f, -50.0f, -50.0f, 1.0f),
                      bspt2};
    SpotLight sp2 = {glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
                      spt2};
    this->lights.spot[2] = sp2;

    BaseLight bspt3 = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                     0.1f,
                     0.1f};
    PointLight spt3 = {glm::vec4(-50.0f, -50.0f, 0.0f, 1.0f),
                      bspt3};
    SpotLight sp3 = {glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
                      spt3};
    this->lights.spot[3] = sp3;

    BaseLight bspt4 = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                     0.1f,
                     0.1f};
    PointLight spt4 = {glm::vec4(210.0f, -50.0f, -50.0f, 1.0f),
                      bspt4};
    SpotLight sp4 = {glm::vec4(-2.0f, 1.0f, 1.0f, 1.0f),
                      spt4};
    this->lights.spot[4] = sp4;

    BaseLight bspt5 = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                     0.1f,
                     0.1f};
    PointLight spt5 = {glm::vec4(-35.0f, -35.0f, 70.0f, 1.0f),
                      bspt5};
    SpotLight sp5 = {glm::vec4(1.0f, 1.0f, -2.0f, 1.0f),
                      spt5};
    this->lights.spot[5] = sp5;

}


GLEmissive::~GLEmissive()
{
}


