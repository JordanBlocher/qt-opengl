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

QGLApp::QGLApp(QWidget *parent) : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent), size(640, 480), timer(this)
{
    setMouseTracking(true);
    setAutoBufferSwap(true);
    timer.setInterval(0);
    connect(&timer, SIGNAL(timeout()), this, SLOT(idleGL()));
}

QGLApp::~QGLApp()
{
    //Auto
}

QSize QGLApp::minimumSizeHint() const
{
    return QSize(40,40);
}

QSize QGLApp::sizeHint() const
{
    return size;
}

void QGLApp::initializeGL()
{
    GLenum status = glewInit();
    if( status != GLEW_OK)
    {
        cerr << "[F] GLEW not initialized: ";
        cerr << glewGetErrorString(status) << std::endl;
        qApp->quit();
        return;
    }
}

bool QGLApp::Add(unique_ptr<GLProgram> program)
{
    glLinkProgram(program->getId());
    this->program.push_front(std::move(program));
    start_time = chrono::high_resolution_clock::now();
    timer.start();

    return this->program.front()->Status();
}

bool QGLApp::Remove()
{
    if( this->program.size() > 0)
    {
        this->program.pop_back();
        return true;
    }
    return false;
}


void QGLApp::paintGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->swapBuffers();
}

void QGLApp::resizeGL(int width, int height)
{
    glViewport( 0, 0, width, height);
    size = QSize(width, height);
}

void QGLApp::idleGL()
{
    updateGL();
}

void QGLApp::keyPressEvent(QKeyEvent *event)
{
   if ( event->key() == Qt::Key_Escape )
   {
      qApp->quit();
   }
}

