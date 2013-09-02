#include <GL/glew.h>
#include <iostream>

#include <QApplication>
#include <QKeyEvent>
#include <string>
#include <fstream>

#include "GLShader.hpp"

using namespace std;

GLShader::GLShader() : path("shaders/")
{
}

GLShader::GLShader(GLenum type) : GLShader()
{
    this->Create(this->toString(type).c_str(), type);
}

GLShader::GLShader(const char* filename, GLenum type) : GLShader()
{
    this->Create(filename, type);
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
        cerr << "[F] Shader failed to compile: " << endl << msg << endl;
        return false;
    }
    return true;
}

bool GLShader::Source(const char* filename)
{
    ifstream fin(path + filename, ios::binary | ios::in);
    if(fin)
    {
        source.resize((fin.seekg(0, ios::end)).tellg());
        fin.seekg(0, ios::beg);
        fin.read(&source.front(), source.size());
        fin.close();
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

string GLShader::toString(GLenum type)
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
