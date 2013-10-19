#include "GLUniform.hpp"
#include "GLBufferObject.hpp"


GLUniform::GLUniform(const char* name) : GLNode(name)
{
}

GLUniform::GLUniform(const char* name, GLuint program, int size, const char* type) : GLNode(name)
{
    this->location = glGetUniformLocation(program, name);
    if(size == 1 && std::string(type) == std::string("i") )
        glProgramUniform1i(program, this->location, 0);
    else if(size == 1 && std::string(type) == std::string("f") )
        glProgramUniform1f(program, this->location, 0);
    else if(size == 2 && std::string(type) == std::string("f") )
        glProgramUniform2f(program, this->location, 0, 0);
    else if(size == 3 && std::string(type) == std::string("f") )
        glProgramUniform3f(program, this->location, 0, 0, 0);
    else if(size == 3 && std::string(type) == std::string("f") )
        glProgramUniform3f(program, this->location, 0, 0, 0);
    else if(size == 4 && std::string(type) == std::string("f") )
        glProgramUniform4f(program, this->location, 0, 0, 0, 0);
    this->id = UINT_MAX;

}

bool GLUniform::CreateUBO(int numUniforms, GLuint program, GLuint index, GLenum draw)
{
    GLint size;
    GLenum type;
    std::string uname;
    GLsizei len;
    GLsizei dataSize = 0;
    GLsizeiptr uniformSize;
    Uniform unif;
    this->location = index;
        
    this->block = glGetUniformBlockIndex(program, this->name.c_str());

    for( int i = 0; i < numUniforms; i++)
    {
        glGetActiveUniform(program, index + i, 256, &len, &size, &type, &uname[0]);

        if(type == GL_FLOAT_VEC3)
            uniformSize = sizeof(glm::vec3);
        else if(type == GL_FLOAT_VEC2)
             uniformSize = sizeof(glm::vec2);
        else if(type == GL_FLOAT_VEC4)
            uniformSize = sizeof(glm::vec4);
        else if(type == GL_FLOAT_MAT4)
            uniformSize = sizeof(glm::mat4);
        else if(type == GL_FLOAT_MAT3)
            uniformSize = sizeof(glm::mat3);
        else if(type == GL_INT)
            uniformSize = sizeof(int);

        dataSize += uniformSize*size;

        unif = {&uname[0], static_cast<GLuint>(size*uniformSize), index + (GLuint)i, static_cast<GLuint>(size*uniformSize*i)};
		this->uniforms[(UniformType)index] = unif;
    }

    GLBufferObject ubo(name.c_str(), dataSize, (GLuint)1, GL_UNIFORM_BUFFER, draw); 
    if( ubo.Status(GL_UNIFORM_BUFFER, dataSize) )
    {
            std::cerr << "[E] Buffer " << name << " not created."<<std::endl;
            return false;
    }
    glBindBufferBase(ubo.Type(), index, ubo.Buffer());
    
    this->id = ubo.Buffer();

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return true;
}

GLUniform::~GLUniform()
{
}

Uniform GLUniform::Get(UniformType type)
{
    return this->uniforms[type];
}

GLuint GLUniform::getLocation()
{
    return this->location;
}


