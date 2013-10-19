#ifndef GLUNIFORM_H
#define GLUNIFORM_H

#include "GLNode.hpp"
#include "GLBufferObject.hpp"
#include "GLStruct.hpp"

#include <vector>
#include <map>
#include <GL/glew.h>

enum UniformType {COLOR = 0, POSITION = 1, NORMAL = 2, UV = 3};  

class GLUniform : public GLNode
{
 public:
     GLUniform(const char*);
     GLUniform(const char*, GLuint, int, const char*);
    ~GLUniform();
    
    bool CreateUBO(int, GLuint, GLuint, GLenum);
    Uniform Get(UniformType);
    GLuint getLocation();

 private:
    std::map<UniformType,Uniform> uniforms;
    GLuint block;
    GLuint location;


};

#endif 

