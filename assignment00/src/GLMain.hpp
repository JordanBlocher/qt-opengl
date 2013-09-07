#ifndef GLMAIN_H
#define GLMAIN_H

#include <QGLApp.hpp>
#include <GLStruct.hpp>

class GLMain : public QGLApp
{
    Q_OBJECT

public:
    GLMain(QWidget *parent = 0);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL();

    GLuint program;
    GLint position;
    GLint color;

    GLuint vbo_geometry;
};

#endif 

