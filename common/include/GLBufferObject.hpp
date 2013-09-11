#ifndef GLBUFFEROBJECT_H
#define GLBUFFEROBJECT_H

#include <string>
#include <memory>

#include "GLNode.hpp"

typedef std::map<std::string, Uniform> UniformMap;
typedef std::shared_ptr<UniformMap> UniformPtr;
typedef std::map<std::string, Vertex> VertexMap;
typedef std::shared_ptr<VertexMap> VertexPtr;

class QGLView;
class GLProgram;

class GLBufferObject : public GLNode
{

 public:
    GLBufferObject(const char*, GLsizeiptr, GLuint, GLuint, GLenum);
    ~GLBufferObject();

    bool Status(GLenum, GLint);
    bool Status();
    void SetBlockIndex(GLuint);
    GLuint getBuffer() const;
    GLenum getType() const;
    Uniform getUniform(const char*);
    void AddUniform(std::pair<std::string, Uniform>);

 protected:
    GLuint buffer;
    GLuint block;
    GLenum type;

    //template<class T>
    //deletePtr<T>(&T)

   // union {
        UniformPtr uniforms; //No union c++11?
        VertexPtr vertices;
   // };
};

/*
template<class T>
void GLBufferObject::deletePtr(T &t)
{
    delete t.second;
}
*/
#endif 

