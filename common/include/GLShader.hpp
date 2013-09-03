#ifndef GLSHADER_H
#define GLSHADER_H

#include "GLNode.hpp"

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
    bool Add(std::string);
    std::string toString(GLenum);

protected:
    std::string source;
    std::string path;
};



#endif 

