#ifndef GLUNIFORM_H
#define GLUNIFORM_H

#include "GLNode.hpp"
#include "GLBufferObject.hpp"
#include "GLStruct.hpp"

#include <vector>
#include <map>
#include <GL/glew.h>

class GLUniform : public GLNode
{
 public:
     GLUniform(const char*);
     GLUniform(const char*, GLuint, int, const char*);
    ~GLUniform();
    
    bool CreateUBO(GLuint, GLuint, GLenum);
    Uniform Get(const char*);
    GLuint getLocation();
    void printUniformOffsets(GLuint, GLuint);

 private:
    std::map<std::string,Uniform> uniforms;
    GLuint block;
    GLuint location;


};

#endif 

