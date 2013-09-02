#ifndef GLMAIN_H
#define GLMAIN_H

#include <QGLApp.hpp>

class GLMain : public QGLApp
{
    Q_OBJECT

public:
    GLMain(QWidget *parent = 0);

protected:
    void initializeGL();
    void paintGL();

    GLuint program;
    GLint position;
    GLint color;
};

#endif 

