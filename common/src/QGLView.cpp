#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <string>

#include <QApplication>
#include <QKeyEvent>
#include <QTimer>
#include <QMenu>
#include <QContextMenuEvent>

#include "QGLView.hpp"
#include "GLNode.hpp"
#include "GLContext.hpp"
#include "GLProgram.hpp"

typedef std::pair<std::string, std::shared_ptr<GLNode>> GLPair;

QGLView::QGLView(QWidget *parent, GLMapPtr m) : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent), GLContext(m), size(640, 480), timer(this)
{
    this->setMouseTracking(true);
    this->setAutoBufferSwap(true);
    this->timer.setInterval(0);
    this->setFocusPolicy(Qt::ClickFocus);
    this->setContextMenuPolicy(Qt::DefaultContextMenu);
    connect(&timer, SIGNAL(timeout()), this, SLOT(idleGL()));
}

QGLView::~QGLView()
{
}

QSize QGLView::minimumSizeHint() const
{
    return QSize(40,40);
}

QSize QGLView::sizeHint() const
{
    return size;
}

void QGLView::initializeGL()
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

void QGLView::paintGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->swapBuffers();
}

void QGLView::resizeGL(int width, int height)
{
    glViewport( 0, 0, width, height);
    size = QSize(width, height);
}

void QGLView::idleGL()
{
    updateGL();
}

void QGLView::keyPressEvent(QKeyEvent *event)
{
   if( event->key() == Qt::Key_Escape )
   {
      qApp->quit();
   }
}

bool QGLView::AddProgram(std::shared_ptr<GLProgram> program)
{
    glLinkProgram(program->getId());
    this->start_time = std::chrono::high_resolution_clock::now();
    GLuint status = program->Status();

    return status;
}

bool QGLView::AddToContext(std::shared_ptr<GLNode> node)
{
    if( GLContext::QGLMap->count(node->getName()) == 0 )
    {
        GLContext::QGLMap->insert(GLPair(node->getName(), node));
        return true;
    }
    return false;
}


void QGLView::contextMenuEvent(QContextMenuEvent *event)
{
    QAction *exit = new QAction(tr("&Quit"), this);
    exit->setShortcuts(QKeySequence::Quit);
    exit->setStatusTip(tr("Quit the application"));
    connect(exit, SIGNAL(triggered()), this, SLOT(quit()));

    QMenu menu(this);    
    menu.addAction(exit);
    menu.exec(event->globalPos());
}

void QGLView::quit()
{
    qApp->quit();
}
