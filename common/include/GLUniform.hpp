#ifndef GLUNIFORM_H
#define GLUNIFORM_H

#include "GLNode.hpp"
#include "GLBufferObject.hpp"
#include "GLStruct.hpp"

#include <vector>
#include <map>

class GLUniform : public GLNode
{
 public:
    GLUniform(const char*);
    ~GLUniform();

    bool CreateUBO(std::vector<std::string>, GLsizeiptr, GLuint, GLuint, GLuint);
    bool SetUniformIndex(const std::vector<std::string>&, GLsizeiptr, GLuint, GLBufferObject &);
    Uniform Get(const char*);

 private:
    std::map<std::string,Uniform> attributes;
    GLuint programId;

};

#endif 

