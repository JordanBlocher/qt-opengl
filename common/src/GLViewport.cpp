#include "GLNode.hpp"
#include "GLViewport.hpp"
#include "GLProgram.hpp"
#include "GLCamera.hpp"

#include <fstream>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include <QApplication>
#include <QKeyEvent>
#include <QTimer>
#include <QMenu>
#include <QContextMenuEvent>
#include <QColor>
#include <QObject>

typedef std::pair<std::string, std::shared_ptr<GLNode>> GLPair;

GLViewport::GLViewport(int width, int height, QWidget *parent, dataPtr context) : QGLWidget(QGLFormat(QGL::HasOverlay | QGL::DoubleBuffer | QGL::DepthBuffer), parent), initialSize(width, height), timer(this)
{
    if(context == NULL)
        this->context = dataPtr(new sceneData);
    else this->context = context;
    this->setMouseTracking(true);
    this->setAutoBufferSwap(true);
    this->timer.setInterval(0);
    this->setFocusPolicy(Qt::ClickFocus);
    this->setContextMenuPolicy(Qt::DefaultContextMenu);
    this->background = Qt::black;
    this->font = Qt::white;

    std::shared_ptr<GLCamera> camera(new GLCamera("camera", this->initialSize));
    this->AddToContext(camera);

    connect(&timer, SIGNAL(timeout()), this, SLOT(idleGL()));

}

GLViewport::~GLViewport()
{
}

QSize GLViewport::minimumSizeHint() const
{
    return QSize(40,40);
}

QSize GLViewport::sizeHint() const
{
    return initialSize;
}

void GLViewport::initializeGL()
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
    emit init();
}

void GLViewport::paintGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->swapBuffers();

    emit paint();
}

void GLViewport::resizeGL(int width, int height)
{
    // Get access to the camera
    std::shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera");

    // Update the size of the viewport and store that
    glViewport( 0, 0, width, height);
    initialSize = QSize(width, height);

    // Update the camera's projection to maintain the aspect ratio
    camera->SetProjection(glm::perspective(45.0f, float(width)/float(height), 0.01f, 100.0f));
}

void GLViewport::idleGL()
{
    updateGL();
}

void GLViewport::keyPressEvent(QKeyEvent *event)
{
    std::shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera");

    // Act on the key press event
    switch(event->key())
    {
        case (Qt::Key_Escape):
            // Close the qApp
            qApp->quit();
            break;
    } 
    emit key(event);
}

void GLViewport::mousePressEvent(QMouseEvent *)
{
}

void GLViewport::updatePaddle(const char*, int)
{
}

bool GLViewport::AddProgram(std::shared_ptr<GLProgram> program)
{
    glLinkProgram(program->getId());
    this->start_time = std::chrono::high_resolution_clock::now();
    GLuint status = program->Status();

    return status;
}

bool GLViewport::AddToContext(const std::shared_ptr<GLNode> node)
{
    if( this->context->count(node->getName()) == 0 )
    {
        this->context->insert(GLPair(node->getName(), node));
        return true;
    }
    return false;
}

void GLViewport::SetScene(const std::shared_ptr<GLNode> node)
{
    this->glScene = node;
}

void GLViewport::contextMenuEvent(QContextMenuEvent *event)
{
    QAction *exit = new QAction(tr("&Quit"), this);
    exit->setShortcuts(QKeySequence::Quit);
    exit->setStatusTip(tr("Quit the application"));
    connect(exit, SIGNAL(triggered()), this, SLOT(quit()));

    QMenu menu(this);    
    menu.addAction(exit);
    menu.exec(event->globalPos());
}

void GLViewport::ViewContext()
{
    std::cout<<"------------ Current Context -------------\n";
    std::map<std::string, std::shared_ptr<GLNode>>::const_iterator mi;
    for (mi = this->context->begin(); mi != this->context->end(); ++mi)
        std::cout<<mi->first<<std::endl;
}

dataPtr GLViewport::getContext()
{
    return this->context;
}

void GLViewport::quit()
{
    qApp->quit();
}

void GLViewport::paintCallback()
{
    this->paintGL();
}

void GLViewport::keyCallback(QKeyEvent *event)
{
    this->keyPressEvent(event);
}

