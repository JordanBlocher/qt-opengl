#ifndef GLMAIN_H
#define GLMAIN_H

#include <GLViewport.hpp>

#include <chrono>

// Also does not belong here
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

class QKeyEvent;
class QContextMenuEvent;
class QColor;

class GLScene : public GLViewport
{
    Q_OBJECT

 public:
    GLScene(QWidget *parent = 0);

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
    bool update;
    float direction;
    QColor background;
    QColor font;

    GLuint vbo_geometry;
    GLuint vbo_geometry2;
    glm::mat4 cube; //obj->world
    glm::mat4 moon;

    static const GLuint V_POSITION = 1;
    static const GLuint V_COLOR = 2;
    static const GLuint U_POSITION = 0;
    static const GLuint NUM_UNIFORMS = 2;

    std::chrono::time_point<std::chrono::high_resolution_clock> time;

};

template <typename T> 
int sgn(T val) 
{
    return (T(0) < val) - (val < T(0));
}

#endif 

