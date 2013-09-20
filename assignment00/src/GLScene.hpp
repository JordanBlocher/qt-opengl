#ifndef GLMAIN_H
#define GLMAIN_H

#include <GLViewport.hpp>

class GLScene : public GLViewport
{
    Q_OBJECT

public:
    GLScene(QWidget *parent = 0);

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

