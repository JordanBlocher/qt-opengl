#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <string>

#include <QApplication>
#include <QKeyEvent>

#include "QGLApp.hpp"
#include "GLNode.hpp"
#include "GLContext.hpp"
#include "GLProgram.hpp"

typedef std::pair<std::string, std::shared_ptr<GLNode>> QGLPair;


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
        std::cerr << "[F] GLEW not initialized: ";
        std::cerr << glewGetErrorString(status) << std::endl;
        qApp->quit();
        return;
    }
    timer.start();
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

std::shared_ptr<GLNode> QGLApp::Get(const char* name) const 
{
    if( GLContext::QGLMap.count(name) == 1 )
        return GLContext::QGLMap.find(name)->second;
    return NULL;
}

bool QGLApp::AddProgram(std::shared_ptr<GLProgram> program)
{
    std::cout<<"set time qglapp "<<program<<std::endl;
    glLinkProgram(program->getId());
    this->start_time = std::chrono::high_resolution_clock::now();
    GLuint status = program->Status();
    this->QGLMap.insert( QGLPair(program->getName(), program));

    return status;
}


