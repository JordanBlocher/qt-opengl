#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include<glm/glm.hpp>

#include "GLNode.hpp"
#include "GLShader.hpp"

class GLProgram : public GLNode
{

public:
    GLProgram();
    GLProgram(GLShader, GLShader);
    ~GLProgram();

    bool Status();
    bool Add(GLShader);
    void Remove(GLShader);
    void Bind(const char*, GLuint);
    void Bind(const char*, GLuint, GLuint, GLenum, Uniform&);
    GLuint getUniforms() const;

};

#endif 

