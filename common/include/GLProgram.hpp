#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include <glm/glm.hpp>
#include <memory>

#include "GLNode.hpp"

class GLShader;

class GLProgram : public GLNode
{

 public:
    GLProgram(const char*);
    GLProgram(std::shared_ptr<GLShader>, std::shared_ptr<GLShader>, const char*);
    ~GLProgram();

    bool Status();
    bool AddShader(std::shared_ptr<GLShader>);
    bool RemoveShader(std::shared_ptr<GLShader>);
    void SetAttributeIndex(const char*, GLuint);
    void SetUniformIndex(const char*, GLuint, GLuint, GLenum, Uniform&);

};

#endif 

