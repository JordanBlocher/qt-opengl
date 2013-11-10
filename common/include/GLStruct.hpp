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
    glm::vec4 emissive;
    glm::vec4 transparency;
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
    glm::vec4 ambientIntensity;
    glm::vec4 diffuseIntensity;
};

struct DirectionalLight
{
    glm::vec4 color;
    glm::vec4 ambientIntensity;
    glm::vec4 diffuseIntensity;
    glm::vec4 direction;
};

struct PointLight
{
    glm::vec4 color;
    glm::vec4 ambientIntensity;
    glm::vec4 diffuseIntensity;
    glm::vec4 position;
    glm::vec4 constant;
    glm::vec4 linear;
    glm::vec4 exp;
};

struct SpotLight
{
    glm::vec4 color;
    glm::vec4 ambientIntensity;
    glm::vec4 diffuseIntensity;
    glm::vec4 direction;
    glm::vec4 cutoff;
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

};

#endif

