#include "GLShader.hpp"

#include <QApplication>
#include <QKeyEvent>
#include <fstream>

GLShader::GLShader(const char* name) : GLNode(name)
{
    path = "assets/shaders/";
}

GLShader::GLShader(GLenum type, const char* name) : GLNode(name)
{
    path = "assets/shaders/";
    this->type = type;
    if( this->Load(this->toString(type).c_str()) )
        this->id = glCreateShader(type);
    this->Compile();
}

GLShader::GLShader(const char* filename, GLenum type, const char* name) : GLNode(name)
{
    path = "assets/shaders/";
    this->type = type;
    if( this->Load(filename) )
        this->id = glCreateShader(type);
    this->Compile();
}

GLShader::~GLShader()
{
    glDeleteShader(this->id);
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

bool GLShader::Load(const char* filename)
{
    std::ifstream fin(path + filename, std::ios::binary | std::ios::in);
    if(fin)
    {
        source.resize((fin.seekg(0, std::ios::end)).tellg());
        fin.seekg(0, std::ios::beg);
        fin.read(&source.front(), source.size());
        fin.close();
        source.append("\0");
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
