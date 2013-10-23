#ifndef GLMAIN_H
#define GLMAIN_H

#include <Entity.hpp>

#include <GLViewport.hpp>

#include <chrono>
#include <vector>


class QKeyEvent;
class QContextMenuEvent;
class QColor;

class GLScene : public GLViewport
{
    Q_OBJECT

 public:
    GLScene(QWidget *parent = 0, int argc = 0, char* argv[] = nullptr);

 protected slots:
    void idleGL();

 protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int, int);
    float getDT();
    void keyPressEvent(QKeyEvent*);
    void mousePressEvent(QMouseEvent*);
    void contextMenuEvent(QContextMenuEvent*);

    QColor background;
    QColor font;

    std::vector<std::string> models;

    std::chrono::time_point<std::chrono::high_resolution_clock> time;
    std::shared_ptr<std::vector<std::shared_ptr<Entity>>> entities;

};

template <typename T> 
int sgn(T val) 
{
    return (T(0) < val) - (val < T(0));
}

#endif 

