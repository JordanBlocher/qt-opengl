#include "GLNode.hpp"
#include "GLViewport.hpp"
#include "GLProgram.hpp"
#include "GLCamera.hpp"

#include <fstream>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <QApplication>
#include <QKeyEvent>
#include <QTimer>
#include <QMenu>
#include <QContextMenuEvent>
#include <QColor>

typedef std::pair<std::string, std::shared_ptr<GLNode>> GLPair;

GLViewport::GLViewport(QWidget *parent) : QGLWidget(QGLFormat(QGL::HasOverlay | QGL::DoubleBuffer | QGL::DepthBuffer), parent), initialSize(640, 480), timer(this), glData(new sceneData)
{
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
}

void GLViewport::paintGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->swapBuffers();
}

void GLViewport::resizeApp(int width, int height)
{
    this->resizeGL(width, height);
}

void GLViewport::resizeGL(int width, int height)
{
    glViewport( 0, 0, width, height);
    initialSize = QSize(width, height);
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
        case (Qt::Key_Right):
            // Move RIGHT
            camera->moveCamera(GLCamera::CamDirection::Right);
            break;    
        case (Qt::Key_Left):
            // Move LEFT
            camera->moveCamera(GLCamera::CamDirection::Left);
            break;
        case (Qt::Key_Up):
            // Forward if SHIFT, UP otherwise
            if(event->modifiers() & Qt::ShiftModifier){
                camera->moveCamera(GLCamera::CamDirection::Forward);
            }
            else
            {
                camera->moveCamera(GLCamera::CamDirection::Up);
            }
            break;
        case (Qt::Key_Down):
            // Backward if SHIFT, DOWN otherwise
            if(event->modifiers() & Qt::ShiftModifier){
                camera->moveCamera(GLCamera::CamDirection::Backward);
            }
            else
            {
                camera->moveCamera(GLCamera::CamDirection::Down);
            }
            break;
    } 

}

void GLViewport::mousePressEvent(QMouseEvent *event)
{
}

void GLViewport::keyPressed(QKeyEvent *event)
{
    this->keyPressEvent(event);
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
    if( this->glData->count(node->getName()) == 0 )
    {
        this->glData->insert(GLPair(node->getName(), node));
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
    for (mi = this->glData->begin(); mi != this->glData->end(); ++mi)
        std::cout<<mi->first<<std::endl;
}

void GLViewport::quit()
{
    qApp->quit();
}
