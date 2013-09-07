#ifndef GLSHADER_H
#define GLSHADER_H

#include "GLNode.hpp"

class GLShader : public GLNode
{
 public:
    GLShader(const char*);
    GLShader(GLenum, const char*);
    GLShader(const char*, GLenum, const char*);
    ~GLShader();

    bool Status();
    bool Compile();

 private:
    void Create(const char*, GLenum);
    bool Source(const char*);
    std::string toString(GLenum);
    std::string source;
    std::string path;


 
};



#endif 

