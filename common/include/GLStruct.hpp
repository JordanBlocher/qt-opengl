#ifndef GLSTRUCT_H
#define GLSTRUCT_H

#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <vector>

struct Material
{
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;
    float intensity;
    float diffuseBlend;
};

struct Uniform
{
    std::string name;
    GLint size;
    GLuint index;
    GLint offset;
};

struct Player
{
    std::string name;
    int score;
    std::string puck;
    bool winner;
};

struct BaseLight
{
    glm::vec4 color;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight
{
    glm::vec4 direction;
    BaseLight base;
};

struct PointLight
{
    glm::vec4 position;
    BaseLight base;
};

struct SpotLight
{
    glm::vec4 direction;
    PointLight point;
};

struct Matrices
{
    glm::mat4 mvpMatrix;
    glm::mat4 mvMatrix;
    glm::mat4 normalMatrix;
};

struct Lights
{
    DirectionalLight basic;
    std::vector<PointLight> point;
    std::vector<SpotLight> spot;

};

#endif

