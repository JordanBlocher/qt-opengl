#include "GLUniform.hpp"
#include "GLBufferObject.hpp"

#include <iomanip>


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

bool GLUniform::CreateUBO(GLuint program, GLuint location, GLenum draw)
{
    GLint size;
    GLint type;
    GLint offset;
    std::string uname;
    GLsizei dataSize = 0;
    GLsizeiptr uniformSize;
    Uniform unif;
    this->location = UINT_MAX;
    GLint numUniforms;
    GLuint index;
      
    this->block = glGetUniformBlockIndex(program, this->name.c_str());
    glUniformBlockBinding(program, block, location);
    glGetActiveUniformBlockiv( program, this->block, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniforms );
    GLint *indices = new GLint[numUniforms];
    glGetActiveUniformBlockiv( program, this->block, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices );

    for( int i = 0; i < numUniforms; i++)
    {
        index = (GLuint)indices[i];

        glGetActiveUniformName(program, index, 256, 0, &uname[0]);
        glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_TYPE, &type);
        glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_OFFSET, &offset);
        glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_SIZE, &size);

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
        else if(type == GL_FLOAT)
            uniformSize = sizeof(float);

        dataSize += static_cast<GLint>(size*uniformSize);

        unif = {&uname[0], static_cast<GLint>(size*uniformSize), index, offset};
		this->uniforms[&uname[0]] = unif;

        std::cout << "Uniform <" << unif.name << "> (offset): " << unif.offset <<", (size): " <<unif.size<< ", (index): "<<unif.index<< std::endl;
        std::cout<<"dataSize : "<<dataSize<<std::endl;
    }

    GLBufferObject ubo(name.c_str(), dataSize, (GLuint)1, GL_UNIFORM_BUFFER, draw); 
    if( ubo.Status(GL_UNIFORM_BUFFER, dataSize) )
    {
            std::cerr << "[E] Buffer " << name << " not created."<<std::endl;
            return false;
    }

    ubo.SetBlockIndex(location); 
    glBindBufferBase(ubo.Type(), location, ubo.Buffer());
    
    this->id = ubo.Buffer();

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

    delete [] indices;

    return true;
}

GLUniform::~GLUniform()
{
}

Uniform GLUniform::Get(const char* name)
{
    return this->uniforms[name];
}

GLuint GLUniform::getLocation()
{
    return this->location;
}


