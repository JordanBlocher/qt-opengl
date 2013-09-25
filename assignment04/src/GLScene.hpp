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
    void mousePressEvent(QMouseEvent*);
    void contextMenuEvent(QContextMenuEvent*);

    bool update;
    QColor background;
    QColor font;

    std::chrono::time_point<std::chrono::high_resolution_clock> time;

};

template <typename T> 
int sgn(T val) 
{
    return (T(0) < val) - (val < T(0));
}

#endif 

