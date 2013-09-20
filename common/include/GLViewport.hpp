#ifndef QGLAPP_H
#define QGLRAPP_H

#include <glm/glm.hpp>
#include <memory>
#include <chrono>
#include <iostream>

#include <QGLWidget>
#include <QTimer>

class GLProgram;
class GLNode;
class QMenu;
class QKeyEvent;
class QContextMenuEvent;
class QColor;
class QPalette;

typedef std::map<std::string, std::shared_ptr<GLNode>> sceneData;
typedef std::shared_ptr<sceneData> dataPtr;

class GLViewport : public QGLWidget
{
Q_OBJECT

 public:
    GLViewport(QWidget *parent = 0);
    ~GLViewport();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void resizeApp(int, int);
    void keyPressed(QKeyEvent*);

 signals:
    void changeDirection(int); 

 protected slots:
    virtual void idleGL();
    virtual void quit();

 protected: 
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int, int);
    virtual void keyPressEvent(QKeyEvent*);
    virtual void contextMenuEvent(QContextMenuEvent*);

    bool AddProgram(std::shared_ptr<GLProgram>);
    bool AddToContext(const std::shared_ptr<GLNode>);
    void SetScene(const std::shared_ptr<GLNode>);
    void ViewContext();
    
    template<class T>
    std::shared_ptr<T> Get(const char*) const;

    QSize initialSize;
    QTimer timer;
    QMenu *popup;
    QColor background;
    QColor font;

    std::shared_ptr<GLNode> glScene;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    glm::mat4 mvp;

    dataPtr glData;
};

template<class T>
std::shared_ptr<T> GLViewport::Get(const char* name) const 
{
    if( this->glData->count(name) == 1 )
        return std::static_pointer_cast<T>(this->glData->find(name)->second);
    else 
        std::cerr << "Node " <<name <<" was not found"<< std::endl;
    return NULL;
}


#endif 

