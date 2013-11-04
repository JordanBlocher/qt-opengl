#ifndef QGLVIEW_H
#define QGLVIEW_H

#include "GLNode.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <chrono>

#include <QGLWidget>
#include <QTimer>

// for std::cerr
#include <iostream>
#include <map>
#include <string>

class GLProgram;
class GLCamera;
class QMenu;
class QKeyEvent;
class QContextMenuEvent;
class QColor;
class QPalette;
struct Dynamics;
struct Player;

typedef std::map<std::string, std::shared_ptr<GLNode>> sceneData;
typedef std::shared_ptr<sceneData> dataPtr;

class GLViewport : public QGLWidget
{
Q_OBJECT

 public:
    GLViewport(int, int, QWidget *parent = 0, dataPtr = NULL);
    ~GLViewport();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    dataPtr getContext();

    template<class T>
    std::shared_ptr<T> Get(const char*) const;

    Player p1;
    Player p2;

 public slots:
    void quit();
    virtual void pause();

 signals:

 protected slots:
    virtual void idleGL();
    virtual void updatePaddle(const char*, int);

 protected: 
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int, int);
    virtual void keyPressEvent(QKeyEvent*);
    virtual void keyReleaseEvent(QKeyEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void contextMenuEvent(QContextMenuEvent*);

    bool AddProgram(std::shared_ptr<GLProgram>);
    bool AddToContext(const std::shared_ptr<GLNode>);
    void SetScene(const std::shared_ptr<GLNode>);
    void ViewContext();
    
    QSize initialSize;
    QTimer timer;
    QMenu *popup;
    QColor background;
    QColor font;

    std::shared_ptr<GLNode> glScene;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    glm::mat4 mvp;

    // Data Structure for Nodes
    dataPtr context;
    // Physics Engine
    std::shared_ptr<Dynamics> dynamics;
    

};

template<class T>
std::shared_ptr<T> GLViewport::Get(const char* name) const 
{
    if( this->context->count(name) == 1 )
        return std::static_pointer_cast<T>(this->context->find(name)->second);
    else 
        std::cerr << "Node " <<name <<" was not found"<< std::endl;
    return NULL;
}


#endif 

