#ifndef GLBUFFEROBJECT_H
#define GLBUFFEROBJECT_H

#include <memory>
#include <vector>

#include "GLNode.hpp"

class GLViewport;
class GLProgram;

class GLBufferObject : public GLNode
{

 public:
    GLBufferObject(const char*, GLsizeiptr, GLuint, GLuint, GLenum, GLenum);
    ~GLBufferObject();

    bool Status(GLenum, GLint);
    bool Status();
    void SetBlockIndex(GLuint);
    GLuint Buffer() const;
    GLenum Type() const;

    template< class T >
    void LoadSubData(size_t, int, const std::vector<T>&);

 protected:
    GLuint buffer;
    GLuint block;
    GLenum type;

};

// Default VBO
template < class T >
void GLBufferObject::LoadSubData(size_t offset, int index, const std::vector<T> &vec)
{
    this->block = index;
    glBufferSubData(this->type, sizeof(T) * offset, sizeof(T) * vec.size(), vec.data());
}


#endif 

