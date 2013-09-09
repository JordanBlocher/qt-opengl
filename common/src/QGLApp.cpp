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

typedef std::pair<std::string, std::shared_ptr<GLNode>> GLPair;

QGLApp::QGLApp(QWidget *parent, GLMapPtr m) : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent), GLContext(m), size(640, 480), timer(this)
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

bool QGLApp::AddProgram(std::shared_ptr<GLProgram> program)
{
    glLinkProgram(program->getId());
    this->start_time = std::chrono::high_resolution_clock::now();
    GLuint status = program->Status();

    return status;
}

bool QGLApp::AddToContext(std::shared_ptr<GLNode> node)
{
    std::cout<<"adding node "<<node->getName()<<std::endl;
    if( GLContext::QGLMap->count(node->getName()) == 0 )
    {
        GLContext::QGLMap->insert(GLPair(node->getName(), node));
        return true;
    }
    return false;
}

