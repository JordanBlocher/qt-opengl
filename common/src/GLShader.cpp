#include <GL/glew.h>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <string>
#include <fstream>

#include "GLShader.hpp"

GLShader::GLShader(const char* name) : GLNode(name)
{
    path = "shaders/";
}

GLShader::GLShader(GLenum type, const char* name) : GLNode(name)
{
    path = "shaders/";
    this->Create(this->toString(type).c_str(), type);
    this->Compile();
}

GLShader::GLShader(const char* filename, GLenum type, const char* name) : GLNode(name)
{
    path = "shaders/";
    this->Create(filename, type);
    this->Compile();
}

GLShader::~GLShader()
{
    glDeleteShader(this->id);
}

void GLShader::Create(const char* filename, GLenum type)
{
    if( this->Source(filename) )
    {
        this->id = glCreateShader(type);
        this->type = type;
    }
}

bool GLShader::Status()
{
    glGetShaderiv(this->id, GL_COMPILE_STATUS, &status);
    if(!status)
    {
        GLint msize;
        glGetShaderiv(this->id, GL_INFO_LOG_LENGTH, &msize);
        msg.resize(msize);
        glGetShaderInfoLog(this->id, msize, NULL, &msg[0]);
        std::cerr << "[F] Shader failed to compile: " << std::endl << msg << std::endl;
        return false;
    }
    return true;
}

bool GLShader::Source(const char* filename)
{
    std::ifstream fin(path + filename, std::ios::binary | std::ios::in);
    if(fin)
    {
        source.resize((fin.seekg(0, std::ios::end)).tellg());
        fin.seekg(0, std::ios::beg);
        fin.read(&source.front(), source.size());
        fin.close();
        source.append("");
        return true;
    }
    return false;
}

bool GLShader::Compile()
{
    const char* source_ptr = source.c_str();
    glShaderSource(this->id, 1, &source_ptr, NULL);
    glCompileShader(this->id);
    if( this->Status() )
        return true;
    return false;
}

std::string GLShader::toString(GLenum type)
{
    switch(type)
    {
        case (GL_VERTEX_SHADER):
            return "vertex.glsl";
            break;
        case (GL_FRAGMENT_SHADER):
            return "fragment.glsl";
            break;
        default:
            return "";
            break;
    }
}
