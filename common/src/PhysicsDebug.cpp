#include "../include/PhysicsDebug.hpp"

#include <glm/glm.hpp>

PhysicsDebug::PhysicsDebug(const char* name) : GLNode::GLNode(name), m_buffersLoaded(false)
{
    glGenVertexArrays(1, &m_vao);
}

PhysicsDebug::~PhysicsDebug()
{}

void PhysicsDebug::draw()
{
    glBindVertexArray(m_vao);
    glDrawArrays(GL_LINES, 0, m_numVertices);
    glBindVertexArray(0);
}

void PhysicsDebug::loadToBuffer()
{
    // delete old buffers
    if ( m_buffersLoaded )
        glDeleteBuffers(2, (GLuint*)(m_buffer));
    
    // generate new buffers
    glGenBuffers(2, (GLuint*)m_buffer);
    m_buffersLoaded = true;

    // initialize buffers
    glBindVertexArray(m_vao);

    // bind vertex locations
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*m_vertices.size(), (void*)m_vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);   // position
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*m_colors.size(), (void*)m_colors.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);   // color
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, 0);

    // bind vertex colors
    glBindVertexArray(0);

    m_numVertices = m_vertices.size();

    // clear the vertices for the next time
    m_vertices.clear();
    m_colors.clear();
}

void PhysicsDebug::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    m_vertices.push_back(glm::vec3(from.x(), from.y(), from.z()));
    m_vertices.push_back(glm::vec3(to.x(), to.y(), to.z()));
    m_colors.push_back(glm::vec3(color.x(), color.y(), color.z()));
    m_colors.push_back(glm::vec3(color.x(), color.y(), color.z()));
}

