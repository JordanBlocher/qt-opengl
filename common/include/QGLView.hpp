#ifndef QGLAPP_H
#define QGLRAPP_H

#include <QGLWidget>

#include <memory>

#include "GLContext.hpp"

class GLProgram;
class GLNode;
class QMenu;
class QTimer;
class QKeyEvent;
class QContextMenuEvent;

typedef std::map<std::string, std::shared_ptr<GLNode>> GLMap;
typedef std::shared_ptr<GLMap> GLMapPtr;

class QGLView : public QGLWidget, public GLContext
{
Q_OBJECT

 public:
    QGLView(QWidget *parent = 0, GLMapPtr m = NULL);
    ~QGLView();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

 protected slots:
    virtual void idleGL();
    virtual void quit();

 protected: 
    virtual void initializeGL();
    virtual void paintGL();
    virtual void keyPressEvent(QKeyEvent*);
    virtual void resizeGL(int, int);
    virtual void contextMenuEvent(QContextMenuEvent*);

    bool AddProgram(std::shared_ptr<GLProgram>);
    bool AddToContext(std::shared_ptr<GLNode>);
    
    template<class T>
    std::shared_ptr<T> Get(const char*) const;

    QSize size;
    QTimer timer;
    QMenu *popup;

};

template<class T>
std::shared_ptr<T> QGLView::Get(const char* name) const 
{
    if( GLContext::QGLMap->count(name) == 1 )
        return std::static_pointer_cast<T>(GLContext::QGLMap->find(name)->second);
    return NULL;
}


#endif 

