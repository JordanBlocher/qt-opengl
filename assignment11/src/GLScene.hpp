#ifndef GLMAIN_H
#define GLMAIN_H

#include <GLViewport.hpp>
#include "SoundManager.hpp"
#include <glm/glm.hpp>
#include "TcpListen.hpp"

class TcpListen;

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
    void tcpDataRcvd(glm::vec3);

 signals:
    void mainMenu(int);
    void dataRcvd(glm::vec3);
    void forceRepaint();
    void updateScore(int, int);
    void endGame();

 protected slots:
    void idleGL();
    void resume();
    void pause();
    void playGame(int);
    void dataWorker(glm::vec3);

 protected:
    void initGame();
    void initializeGL();
    void paintGL();
    void resizeGL(int, int);
    float getDT();
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
    void updateKeys(float dt);
    void mousePressEvent(QMouseEvent*);
    void contextMenuEvent(QContextMenuEvent*);
    void wheelEvent(QWheelEvent*);
    void updateBallGravVector(float dt);
    void checkGameState();
    void DebugDrawInit();
    void DebugDraw();
    void DebugPhysicsInit();
    void DebugPhysicsDraw();

    bool keyHeld[16];// W S A D I K J L R RG LF UP DW NP_8 NP_2 NP_4 NP_6 
    bool update;
    QColor background;
    QColor font;
    std::vector<std::string> gameLevels;
    std::vector<std::string> balls;
    std::vector<glm::vec3> startPosition;
    std::vector<glm::vec3> endPosition;
    size_t levelIdx;
    bool damping;
    int levelCount;
    float elapsedTime; // Not good, sry

    float tablePitch, tableRoll;

    std::chrono::time_point<std::chrono::high_resolution_clock> time;

    std::shared_ptr<std::vector<std::shared_ptr<Entity>>> entities;
    TcpListen *tcpListen;
};

template <typename T> 
int sgn(T val) 
{
    return (T(0) < val) - (val < T(0));
}

#endif 

