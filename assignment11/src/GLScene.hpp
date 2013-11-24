#ifndef GLMAIN_H
#define GLMAIN_H

#include <GLViewport.hpp>
#include "SoundManager.hpp"

class QKeyEvent;
class QContextMenuEvent;
class QColor;
class Entity;

class GLScene : public GLViewport
{
    Q_OBJECT

 public:
    GLScene(int, int, QWidget *parent = 0, int argc = 0, char* argv[] = nullptr);
    ~GLScene();

 signals:
    void mainMenu(int);

 protected slots:
    void idleGL();
    void resume();
    void pause();
    void playGame(int);

 protected:
    void initGame();
    void initializeGL();
    void paintGL();
    void resizeGL(int, int);
    float getDT();
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
    void updateKeys();
    void mousePressEvent(QMouseEvent*);
    void contextMenuEvent(QContextMenuEvent*);
    void wheelEvent(QWheelEvent*);

    bool keyHeld[12];// W S A D I K J L R RG LF UP DW
    bool update;
    QColor background;
    QColor font;
    std::vector<std::string> gameLevels;
    size_t levelIdx;

    std::chrono::time_point<std::chrono::high_resolution_clock> time;

    std::shared_ptr<std::vector<std::shared_ptr<Entity>>> entities;
};

template <typename T> 
int sgn(T val) 
{
    return (T(0) < val) - (val < T(0));
}

#endif 

