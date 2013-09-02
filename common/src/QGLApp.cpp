#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <stdio.h>

#include <QApplication>
#include <QKeyEvent>

#include "QGLApp.hpp"

using namespace std;

QGLApp::QGLApp(QWidget *parent) : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent), window_size(640, 480)
{
    setMouseTracking(true);
}

QGLApp::~QGLApp()
{
    // list of programs is auto destroyed
}

QSize QGLApp::minimumSizeHint() const
{
    return QSize(40,40);
}

QSize QGLApp::sizeHint() const
{
    return window_size;
}

void QGLApp::initializeGL()
{
    GLenum status = glewInit();
    if( status != GLEW_OK)
    {
        cerr << "[F] GLEW NOT INITIALIZED: ";
        cerr << glewGetErrorString(status) << std::endl;
    }
}

bool QGLApp::Add(unique_ptr<GLProgram> program)
{
    glLinkProgram(program->getId());
    this->program.push_front(std::move(program));
    return this->program.front()->Status();
}

void QGLApp::paintGL()
{
    //clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    this->swapBuffers();
}

void QGLApp::keyPressEvent(QKeyEvent *event)
{
   // Handle keyboard input
   if ( event->key() == Qt::Key_Escape )
   {
      qApp->quit();
   }
}

