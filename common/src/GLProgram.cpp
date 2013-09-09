#include <GL/glew.h>
#include <iostream>
#include <glm/glm.hpp>

#include "GLProgram.hpp"
#include "GLShader.hpp"
#include "GLBufferObject.hpp"

GLProgram::GLProgram(const char* name) : GLNode(name)
{
    this->id = glCreateProgram();
}

GLProgram::GLProgram(std::shared_ptr<GLShader> vertex, std::shared_ptr<GLShader> fragment, const char* name) : GLNode(name)
{
    this->id = glCreateProgram();
    this->AddShader(vertex);
    this->AddShader(fragment);
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
        std::cerr << msg << std::endl;
        return false;
    }
    return true;
}

bool GLProgram::AddShader(std::shared_ptr<GLShader> shader)
{
    if(shader->Status())
    {
        glAttachShader(this->id, shader->getId());
    }
    return this->Status();
}

bool GLProgram::RemoveShader(std::shared_ptr<GLShader> shader)
{
    //TODO: Verify shader is flagged for delete after attach
    glGetShaderiv(shader->getId(), GL_DELETE_STATUS, &status);
    if(status)
    {
        glDetachShader(this->id, shader->getId());
        return true;
    }
    return false;
}

void GLProgram::SetAttributeIndex(const char* name, GLuint index)
{
    glBindAttribLocation(this->id, index, name);
}

bool GLProgram::SetUniformIndex(std::shared_ptr<GLBufferObject> ubo, const std::vector<std::string> &names, GLsizeiptr type, GLuint index)
{

    if(ubo == NULL)
    {
        std::cerr << "[F] UBO not found"<< std::endl;
        return false;
    }
    if(ubo->Status())
    {
        std::cerr <<"[W] Buffer " << index <<" is not empty"<< std::endl;
    }

    GLint numUniforms;
    GLuint block;
    std::vector<GLuint> dBlockUniforms;
    std::vector<GLint> size;

    glGetProgramiv(this->id, GL_ACTIVE_UNIFORMS, &numUniforms);

    for(int i=0; i < numUniforms; i++)
    {
        dBlockUniforms.push_back(i);
    }

    size.resize(numUniforms);
    glGetActiveUniformsiv(this->id, numUniforms, dBlockUniforms.data(), GL_UNIFORM_SIZE, size.data());

    if( names.size() > numUniforms )
    {
        std::cerr << "Size of name array is outside range of active uniforms" << std::endl;
        return false;
    }
    
    block = glGetUniformBlockIndex(this->id, (ubo->getName()).c_str());
    ubo->SetBlockIndex(block);
    
    for( int i = 0; i < numUniforms; i++)
    {
        
        Uniform uniform(size[i]*type, index + i, size[i]*type*i);
        std::pair<std::string, Uniform> pair(names[i], uniform);
        ubo->AddUniform(pair);
    }


   glUniformBlockBinding(this->id, block, index);
   glBindBufferBase(ubo->getType(), index, ubo->getBuffer()); 

    return true;
}

