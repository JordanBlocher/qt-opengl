#ifndef GLMAIN_H
#define GLMAIN_H

#include <QGLView.hpp>

#include <chrono>

// Also does not belong here
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

class QKeyEvent;
class QContextMenuEvent;

class GLMain : public QGLView
{
    Q_OBJECT

 public:
    GLMain(QWidget *parent = 0);

 protected slots:
    void start();
    void stop();
    void idleGL();

 protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int, int);
    float getDT();
    void keyPressEvent(QKeyEvent*);
    void contextMenuEvent(QContextMenuEvent*);

    float angle;
    float rotate;
    float direction;
    bool update;

    // Stuff that does not belong here
    static const GLuint V_POSITION = 1;
    static const GLuint V_COLOR = 2;
    static const GLuint U_POSITION = 0;
    static const GLuint NUM_UNIFORMS = 2;

    GLuint vbo_geometry;
   
    //transform matrices
    glm::mat4 model;//obj->world each object should have its own model matrix
    glm::mat4 view;//world->eye
    glm::mat4 projection;//eye->clip
    glm::mat4 mvp;//premultiplied modelviewprojection
    std::chrono::time_point<std::chrono::high_resolution_clock> time;

};

template <typename T> 
int sgn(T val) 
{
    return (T(0) < val) - (val < T(0));
}

#endif 

