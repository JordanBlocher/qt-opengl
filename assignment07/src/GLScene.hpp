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
    GLScene(int, int, QWidget *parent = 0, int argc = 0, char* argv[] = nullptr);
    std::shared_ptr<std::vector<std::shared_ptr<Entity>>> getEntities();

 signals:
    void updateScore(int, int);
    void mainMenu();

 protected slots:
    virtual void idleGL();
    virtual void updatePaddle(const char*, int);
    virtual void playGame(int);

 protected:
    virtual void initGame();
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int, int);
    virtual float getDT();
    virtual void keyPressEvent(QKeyEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void contextMenuEvent(QContextMenuEvent*);

    QColor background;
    QColor font;

    std::vector<std::string> staticModels;
    std::vector<std::string> dynamicModels;

    std::chrono::time_point<std::chrono::high_resolution_clock> time;
    std::shared_ptr<std::vector<std::shared_ptr<Entity>>> entities;

};

#endif 

