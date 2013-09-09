#ifndef QGLAPP_H
#define QGLRAPP_H

#include <QGLWidget>
#include <QTimer>

#include <memory>

#include "GLContext.hpp"

class GLProgram;
class GLNode;

typedef std::map<std::string, std::shared_ptr<GLNode>> GLMap;
typedef std::shared_ptr<GLMap> GLMapPtr;

class QGLApp : public QGLWidget, public GLContext
{
Q_OBJECT

 public:
    QGLApp(QWidget *parent = 0, GLMapPtr m = NULL);
    ~QGLApp();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

 protected slots:
    virtual void idleGL();

 protected: 
    virtual void initializeGL();
    virtual void paintGL();
    virtual void keyPressEvent(QKeyEvent*);
    virtual void resizeGL(int, int);

    bool AddProgram(std::shared_ptr<GLProgram>);
    bool AddToContext(std::shared_ptr<GLNode>);
    
    template<class T>
    std::shared_ptr<T> Get(const char*) const;

    QSize size;
    QTimer timer;

};

template<class T>
std::shared_ptr<T> QGLApp::Get(const char* name) const 
{
    if( GLContext::QGLMap->count(name) == 1 )
        return std::static_pointer_cast<T>(GLContext::QGLMap->find(name)->second);
    return NULL;
}


#endif 

