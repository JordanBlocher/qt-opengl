#define CHECKGLERROR if ( glGetError() != GL_NO_ERROR ) cout << __LINE__ << endl;

#include <GL/glew.h>

#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QPalette>
#include <QColor>

#include <iostream>
#include <chrono>
#include <csignal>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

#include <GLShader.hpp>
#include <GLProgram.hpp>
#include <GLBufferObject.hpp>
#include <GLTransform.hpp>
#include <GLModel.hpp>
#include <GLUniform.hpp>
#include <GLPrint.hpp>

#include "GLScene.hpp"

#define FOV 45.0f
#define SENSOR_DISTANCE 0.01f
#define FOCAL_DISTANCE 100.0f

const GLuint NUM_UNIFORMS = 1;
const GLuint NUM_ATTRIBUTES = 1;
const GLuint NUM_COLORS = 1;
const GLuint POSITION_OFFSET = 0;
const GLuint COLOR_OFFSET = 1;


using namespace std;

GLScene::GLScene(QWidget *parent, int argc, char* argv[]) : GLViewport(parent), update(true), background(QColor::fromRgbF(0.0, 0.0, 0.2)), font(Qt::white)
{
    this->modelpath = argv[1];
    if(argc > 2)
        this->materialpath = argv[2];
    this->setContextMenuPolicy(Qt::DefaultContextMenu);    
    // Projection matrix (needed by resize)
    shared_ptr<GLTransform> perspective(new GLTransform("projection"));
    this->AddToContext(perspective);
}

void GLScene::initializeGL()
{
 
    GLViewport::initializeGL();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    string filename;

    //Models
    shared_ptr<GLModel> model(new GLModel(this->modelpath.c_str(), "model", NUM_ATTRIBUTES));

    if( model->CreateVAO() )
        this->AddToContext(model);
    else qApp->quit();

    //Shaders
    shared_ptr<GLShader> vertex(new GLShader(GL_VERTEX_SHADER, "vshader"));
    shared_ptr<GLShader> fragment(new GLShader(GL_FRAGMENT_SHADER, "fshader"));

    //Program
    shared_ptr<GLProgram> program(new GLProgram("qgl_program"));
    
    //Add Shaders
    if( !program->AddShader(vertex) )
        qApp->quit();
    if( !program->AddShader(fragment) )
        qApp->quit();

    //Bind attribute index
    program->SetAttributeIndex("v_position", V_INDEX);

    // View matrix (eye pos, focus point, up)
    shared_ptr<GLTransform> view( new GLTransform("view", glm::lookAt(
                                    glm::vec3(0.0, 8.0, -16.0),
                                    glm::vec3(0.0, 0.0, 0.0),
                                    glm::vec3(0.0, 1.0, 0.0))));
    this->AddToContext(view);

    // Set Projection matrix
    shared_ptr<GLTransform> projection = this->Get<GLTransform>("projection");
    projection->Set(glm::perspective(
                    FOV, 
                    float(this->size().width())
                    /float(this->size().height()),
                    SENSOR_DISTANCE,
                    FOCAL_DISTANCE) );

    //Add Program
    if( this->AddProgram(program) )
        this->AddToContext(program);
    else qApp->quit();
    
    //Create UBOs
    vector<string> ufragment = {"color"};
    std::shared_ptr<GLUniform> vertex_uniform(new GLUniform("GColors"));
    if( vertex_uniform->CreateUBO(ufragment, sizeof(glm::vec3), COLOR_OFFSET, GL_STATIC_DRAW, program->getId()) )
       this->AddToContext(vertex_uniform);
    else qApp->quit();
    
    vector<string> uvertex = {"mvpMatrix"};
    std::shared_ptr<GLUniform> frag_uniform(new GLUniform("GMatrices"));
    if( frag_uniform->CreateUBO(uvertex, sizeof(glm::mat4), POSITION_OFFSET, GL_STREAM_DRAW, program->getId()) )
       this->AddToContext(frag_uniform);
    else qApp->quit();
    

    //this->SetScene(perspective);

}

void GLScene::paintGL()
{
    //Clear the screen
    qreal r, g, b;
    this->background.getRgbF(&r, &g, &b);
    glClearColor(r, g, b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Choose the shader program
    shared_ptr<GLProgram> program = this->Get<GLProgram>("qgl_program");
    glUseProgram(program->getId());
    
    //Get view & projection matrices
    shared_ptr<GLTransform> projection = this->Get<GLTransform>("projection");
    shared_ptr<GLTransform> view = this->Get<GLTransform>("view");

    //Choose Model
    shared_ptr<GLModel> model = this->Get<GLModel>("model");

    glm::mat4 vp = projection->Matrix() * view->Matrix();
 
    //Get Shader Data
    shared_ptr<GLUniform> vertex_uniform = this->Get<GLUniform>("GMatrices");
    shared_ptr<GLUniform> frag_uniform = this->Get<GLUniform>("GColors");

    //Draw model
    model->Draw(vp, vertex_uniform, frag_uniform);

    glUseProgram(0);

}

void GLScene::idleGL()
{
    if( update )
    {
        GLViewport::updateGL();
    }
}

void GLScene::resizeGL(int width, int height)
{
    GLViewport::resizeGL(width, height);
    shared_ptr<GLTransform> projection = this->Get<GLTransform>("projection");
    projection->Set(glm::perspective(FOV, float(width)/float(height), SENSOR_DISTANCE, FOCAL_DISTANCE));
}

float GLScene::getDT()
{
    float ret;
    time = chrono::high_resolution_clock::now();
    ret = chrono::duration_cast< std::chrono::duration<float> >(time-this->start_time).count();
    this->start_time = chrono::high_resolution_clock::now();
    return ret;
}

void GLScene::keyPressEvent(QKeyEvent *event)
{
    if( (event->key() == Qt::Key_Right))
    {
        shared_ptr<GLTransform> view = this->Get<GLTransform>("view");
        view->Set(glm::rotate( view->Matrix() * glm::mat4(1.0f), 15.0f, glm::vec3(0.0, 1.0, 0.0)) );
    }
    else if( (event->key() == Qt::Key_Left))
    {
        shared_ptr<GLTransform> view = this->Get<GLTransform>("view");
        view->Set(glm::rotate( view->Matrix() * glm::mat4(1.0f), -15.0f, glm::vec3(0.0, 1.0, 0.0)) );
    }
    else if( (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_Up)
    {
        shared_ptr<GLModel> model = this->Get<GLModel>("model");
        model->setMatrix( glm::scale(glm::mat4(1.0f), glm::vec3(1.25, 1.25, 1.25)) * model->Matrix());
    }
    else if( (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_Down)
    {
        shared_ptr<GLModel> model = this->Get<GLModel>("model");
        model->setMatrix( glm::scale(glm::mat4(1.0f), glm::vec3(1.0/1.25, 1.0/1.25, 1.0/1.25)) * model->Matrix());
    }
    else if( (event->key() == Qt::Key_Up))
    {
        shared_ptr<GLTransform> view = this->Get<GLTransform>("view");
        view->Set( glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -2.0, 4.0)) * view->Matrix());
    }
    else if( (event->key() == Qt::Key_Down))
    {
        shared_ptr<GLTransform> view = this->Get<GLTransform>("view");
        view->Set( glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 2.0, -4.0)) * view->Matrix());
    }
        GLViewport::keyPressEvent(event);
}

void GLScene::mousePressEvent(QMouseEvent *event)
{
   event->accept();
}

void GLScene::contextMenuEvent(QContextMenuEvent *event)
{
    QAction *start = new QAction(tr("&Start"), this);
    start->setStatusTip(tr("Start"));
    connect(start, SIGNAL(triggered()), this, SLOT(start()));
    
    QAction *stop = new QAction(tr("&Stop"), this);
    start->setStatusTip(tr("Stop"));
    connect(stop, SIGNAL(triggered()), this, SLOT(stop()));

    QMenu menu(this);    
    menu.addAction(start);
    menu.addAction(stop);
    menu.exec(event->globalPos());
}

void GLScene::start()
{
    if(!this->update)
    {
        GLViewport::start_time  = std::chrono::high_resolution_clock::now();
        this->update = true;
        this->time = std::chrono::high_resolution_clock::now();
        GLViewport::timer.start();
    }
}

void GLScene::stop()
{
    if(this->update)
    {
        this->update = false;
        GLViewport::timer.stop();
    }
}

