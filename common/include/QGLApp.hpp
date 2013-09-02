#ifndef QGLAPP_H
#define QGLRAPP_H

#include <QGLWidget>
#include <memory>

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

    //Object Functions
    virtual bool Add(unique_ptr<GLProgram>);
//    virtual bool Remove(GLProgram);

protected:
    //Callbacks
    virtual void initializeGL();
    virtual void paintGL();
    virtual void keyPressEvent(QKeyEvent*);

    struct Vertex
    {
        GLfloat position[3];
        GLfloat color[3];
    };

    QSize window_size;
    GLuint vbo_geometry;

    list<std::unique_ptr<GLProgram>> program;

};

#endif 

