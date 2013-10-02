#include "GLUniform.hpp"
#include "GLBufferObject.hpp"

GLUniform::GLUniform(const char* name) : GLNode(name)
{
}

bool GLUniform::CreateUBO(std::vector<std::string> attributes, GLsizeiptr size, GLuint offset, GLuint draw, GLuint program)
{
    GLBufferObject ubo(this->name.c_str(), size, attributes.size(), offset, GL_UNIFORM_BUFFER, draw);
    this->id = ubo.Buffer();
    this->programId = program;
    return this->SetUniformIndex(attributes, size, offset, ubo);
}

GLUniform::~GLUniform()
{
}

bool GLUniform::SetUniformIndex(const std::vector<std::string> &names, GLsizeiptr type, GLuint index, GLBufferObject &ubo)
{
    if(ubo.Status())
    {
        std::cerr <<"[W] Buffer " << index <<" is not empty"<< std::endl;
        return false;
    }

    GLint numUniforms;
    GLuint block;
    std::vector<GLuint> dBlockUniforms;
    std::vector<GLint> size;

    glGetProgramiv(this->programId, GL_ACTIVE_UNIFORMS, &numUniforms);

    for(int i=0; i < numUniforms; i++)
    {
        dBlockUniforms.push_back(i);
    }

    size.resize(numUniforms);
    glGetActiveUniformsiv(this->id, numUniforms, dBlockUniforms.data(), GL_UNIFORM_SIZE, size.data());

    if( (int)names.size() > numUniforms )
    {
        std::cerr << "Size of name array is outside range of active uniforms" << std::endl;
        return false;
    }
    
    block = glGetUniformBlockIndex(this->id, (ubo.getName()).c_str());
    ubo.SetBlockIndex(block);
    
    for( int i = 0; i < numUniforms; i++)
    {
        Uniform unif = {(GLuint)(size[i]*type), index + i, (GLuint)(size[i]*type*i)};
        this->attributes[names[i]] = unif;
    }

    glUniformBlockBinding(this->id, block, index);
    glBindBufferBase(ubo.Type(), index, ubo.Buffer()); 

    return true;
}

Uniform GLUniform::Get(const char* name)
{
    return this->attributes[name];
}


