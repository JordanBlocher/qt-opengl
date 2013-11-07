#ifndef GLSTRUCT_H
#define GLSTRUCT_H

#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

struct Material
{
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    glm::vec3 transparent;
    float transparency;
    bool texture;
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

struct Light
{
    float intensity;
    glm::vec3 color;
};

struct Matrices
{
    glm::mat4 mvpMatrix;
    glm::mat4 mvMatrix;
    glm::mat4 normalMatrix;
};

#endif

