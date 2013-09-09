#ifndef GLMAIN_H
#define GLMAIN_H

#include <QGLApp.hpp>
#include <chrono>

// Also does not belong here
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

class GLMain : public QGLApp
{
    Q_OBJECT

public:
    GLMain(QWidget *parent = 0);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int, int);
    void idleGL();
    float getDT();

    float angle;

    // Stuff that does not belong here
    static const GLuint V_POSITION = 1;
    static const GLuint V_COLOR = 2;
    static const GLuint U_POSITION = 0;
    static const GLuint NUM_UNIFORMS = 2;

    static constexpr float FOV = 45.0f; // half-angle FOV
    static constexpr float sensor_distance = 0.01f;
    static constexpr float focal_distance = 100.0f;

    GLuint vbo_geometry;
   
    //transform matrices
    glm::mat4 model;//obj->world each object should have its own model matrix
    glm::mat4 view;//world->eye
    glm::mat4 projection;//eye->clip
    glm::mat4 mvp;//premultiplied modelviewprojection
    std::chrono::time_point<std::chrono::high_resolution_clock> time;

};

#endif 

