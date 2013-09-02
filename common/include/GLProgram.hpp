#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include "GLNode.hpp"
#include "GLShader.hpp"

using namespace std;

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

};

#endif 

