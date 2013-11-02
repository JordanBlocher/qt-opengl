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
    GLScene(int, int, QWidget *parent = 0, int argc = 0, char* argv[] = nullptr);

 signals:
    void mainMenu(int);
    void updateScore(int, int);

 protected slots:
    void playGame(int);
    void changePaddle(int);
    void resume();
    void pause();
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
    std::string modelpath;
    std::string materialpath;

    std::chrono::time_point<std::chrono::high_resolution_clock> time;

};

template <typename T> 
int sgn(T val) 
{
    return (T(0) < val) - (val < T(0));
}

#endif 

