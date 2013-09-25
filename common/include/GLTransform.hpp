#ifndef GLTRANSFORM_H
#define GLTRANSFORM_H

#include <glm/glm.hpp>

#include "GLNode.hpp"

class GLTransform : public GLNode
{
 
 public:
    GLTransform(const char*);
    GLTransform(const char*, glm::mat4);
    ~GLTransform();

    void Transform(glm::mat4);
    glm::mat4 Matrix();
    void Set(glm::mat4);

protected:
    glm::mat4 matrix;

};

#endif 

