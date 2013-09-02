#include <GL/glew.h>
#include <iostream>

#include <QApplication>
#include <QKeyEvent>

#include "GLProgram.hpp"

using namespace std;


GLProgram::GLProgram()
{
    this->id = glCreateProgram();
}

GLProgram::GLProgram(GLShader vertex, GLShader fragment) : GLProgram()
{
    this->Add(vertex);
    this->Add(fragment);
}

GLProgram::~GLProgram()
{
    glDeleteProgram(this->id);
}

bool GLProgram::Status()
{
    glGetProgramiv(this->id, GL_LINK_STATUS, &status);
    if(!status)
    {
        GLint msize;
        glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &msize);
        msg.resize(msize);
        glGetProgramInfoLog(this->id, msize, NULL, &msg[0]);
        cerr << msg << endl;
        return false;
    }
    return true;
}

bool GLProgram::Add(GLShader shader)
{
    if(shader.Compile())
    {
        glAttachShader(this->id, shader.getId());
        return true;
    }
    return false;
}

void GLProgram::Remove(GLShader shader)
{
    glDetachShader(this->id, shader.getId());
}

void GLProgram::Bind(const char* attribute, GLuint index)
{
    glBindAttribLocation(this->id, index, attribute);
}
