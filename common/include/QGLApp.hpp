#ifndef QGLAPP_H
#define QGLRAPP_H

#include <QGLWidget>
#include <QTimer>

#include <memory>

#include "GLContext.hpp"

class GLProgram;
class GLNode;

class QGLApp : public QGLWidget, public GLContext
{
Q_OBJECT

 public:
    QGLApp(QWidget *parent = 0);
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

    virtual bool AddProgram(std::shared_ptr<GLProgram>);
    virtual std::shared_ptr<GLNode> Get(const char*) const;


    QSize size;
    QTimer timer;

};

#endif 

