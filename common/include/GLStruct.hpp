#include <iostream>

struct Uniform
{
    GLuint size;
    GLuint index;
    GLuint offset;
    
    Uniform(GLuint s, GLuint i, GLuint o) : size(s), index(i), offset(o){ std::cout<<"constructing uniform"<<std::endl;}
};

struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};

struct Triangle
{
    Vertex x;
    Vertex y;
    Vertex z;
};

