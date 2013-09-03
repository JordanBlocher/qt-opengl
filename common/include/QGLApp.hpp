#ifndef QGLAPP_H
#define QGLRAPP_H

#include <QGLWidget>
#include <QTimer>

#include <memory>
#include <chrono>

#include "GLNode.hpp"
#include "GLProgram.hpp"

using std::unique_ptr;

class QGLApp : public QGLWidget
{
Q_OBJECT

public:
    QGLApp(QWidget *parent = 0);
    ~QGLApp();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    virtual bool Add(unique_ptr<GLProgram>);
    virtual bool Remove();

    Uniform GLUniform;

protected slots:
    virtual void idleGL();

protected: 
    //Callbacks
    virtual void initializeGL();
    virtual void paintGL();
    virtual void keyPressEvent(QKeyEvent*);
    virtual void resizeGL(int, int);

    QSize size;
    QTimer timer;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

    std::list<std::unique_ptr<GLProgram>> program;
    std::list<std::unique_ptr<GLShader>> shader;
    
 };

#endif 

