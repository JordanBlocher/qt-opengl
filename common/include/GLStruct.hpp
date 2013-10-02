#include <iostream>
#include <glm/glm.hpp>
struct Vertex
{
    //GLfloat
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Material
{
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float transparency;
    
};

