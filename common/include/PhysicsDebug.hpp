#ifndef DEBUGDRAW_HPP
#define DEBUGDRAW_HPP

#include <bullet/LinearMath/btIDebugDraw.h>
#include "GLNode.hpp"

class PhysicsDebug : public btIDebugDraw, public GLNode
{
public:
    PhysicsDebug(const char* name);
    ~PhysicsDebug();

    void draw();
    void loadToBuffer();

    // overloaded pure virtual from btIDebugDraw
    void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);

    void drawContactPoint(const btVector3&,const btVector3&,btScalar,int,const btVector3&) {}
    void reportErrorWarning(const char*) {}
    void draw3dText(const btVector3&,const char*) {}
    void setDebugMode(int) {}
    int getDebugMode() const { return btIDebugDraw::DBG_DrawWireframe; }
    
    // originals with argument names
    //void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) {}
    //void reportErrorWarning(const char* warningString) {}
    //void draw3dText(const btVector3& location,const char* textString) {}
    //void setDebugMode(int debugMode) {}
protected:
    GLsizei                         m_numVertices;
    
    std::vector<glm::vec3>          m_vertices;
    std::vector<glm::vec3>          m_colors;
   
    GLuint                          m_vao;

    GLint                           m_buffer[2];
    bool                            m_buffersLoaded;

    // always identity
    glm::mat4                       m_modelMatrix;
};

#endif // DEBUGDRAW_HPP

