#ifndef GLSHADER_H
#define GLSHADER_H

#include "GLNode.hpp"

using namespace std;

class GLShader : public GLNode
{

public:
    GLShader();
    GLShader(GLenum);
    GLShader(const char*, GLenum);
    ~GLShader();

    bool Compile();
    void Create(const char*, GLenum);
    bool Status();
    bool Source(const char*);
    bool Add(string);
    string toString(GLenum);

protected:
    string source;
    string path;
};



#endif 

