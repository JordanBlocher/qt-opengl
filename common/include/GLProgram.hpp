#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include "GLNode.hpp"

#include <vector>

class GLShader;
class GLBufferObject;
class GLUniform;

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
    std::vector<std::shared_ptr<GLUniform>> SetUniformIndex(std::shared_ptr<GLBufferObject>, const std::vector<std::string>&, GLsizeiptr, GLuint);

};

#endif 

