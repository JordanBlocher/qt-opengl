#ifndef GLMAIN_H
#define GLMAIN_H

#include <Entity.hpp>

#include <GLViewport.hpp>
#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include <phonon/MediaSource>
#include <phonon/VideoWidget>
#include <phonon/VideoPlayer>

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
    ~GLScene();
    std::shared_ptr<std::vector<std::shared_ptr<Entity>>> getEntities();

 signals:
    // updateScore(score, player)
    void updateScore(int, int); 
    void mainMenu(int);
    void playSound(int soundNum);
    void endGame();

 protected slots:
    void resume();
    void pause();
    void idleGL();
    void changePaddle(int);
    void playGame(int);
    void playSoundSlot(int soundNum);    

 protected:
    void initGame();
    void addStaticBodies();
    void addDynamicBodies();
    void removeDynamicBodies();
    void initializeGL();
    void paintGL();
    void resizeGL(int, int);
    float getDT();
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void contextMenuEvent(QContextMenuEvent*);
    void updateKeys();

    void monitorProps();
    void invokeAI();
    void monitorScore();

    QColor background;
    QColor font;
    bool update;
    int numPlayers;
    int puckIndex;
    int paddleIndex;
    int player1Score, player2Score;
    bool keyHeld[12];// W S A D I K J L R RG LF UP DW
    bool aiOnline;
    
    std::vector<std::string> puckTypes;
    std::vector<std::string> paddleTypes;

    std::chrono::time_point<std::chrono::high_resolution_clock> time;
    std::shared_ptr<std::vector<std::shared_ptr<Entity>>> entities;

    // Phonon
    Phonon::MediaObject *mediaObject;
    Phonon::MediaObject *metaInformationResolver;
    Phonon::AudioOutput *audioOutput;
    QList<Phonon::MediaSource> sources;

    bool playingBg;
    qint64 bgSeekTime;
};

#endif 

