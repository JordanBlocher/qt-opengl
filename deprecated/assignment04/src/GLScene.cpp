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

const GLuint V_INDEX = 1; //Vertex attribute indices start at 1
const GLuint POSITION_OFFSET = 0;
const GLuint COLOR_OFFSET = 1;
const GLuint NUM_UNIFORMS = 1;
const GLuint NUM_COLORS = 1;


using namespace std;

GLScene::GLScene(QWidget *parent) : GLViewport(parent), update(true), background(QColor::fromRgbF(0.0, 0.0, 0.2)), font(Qt::white)
{
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
    cout<<"Enter model path:"<<endl;
    cin>> filename;

    shared_ptr<GLModel> model(new GLModel(filename.c_str(), "model"));
    shared_ptr<vector<glm::vec3>> positions = model->Positions();
    this->AddToContext(model);

    //VBO (positions)
    shared_ptr<GLBufferObject> vbo(new GLBufferObject(
                                   "vboposition",
                                   sizeof(glm::vec3),
                                   positions->size(),
                                   POSITION_OFFSET,
                                   GL_ARRAY_BUFFER,
                                   GL_STATIC_DRAW) );
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo->Buffer());
    glBufferSubData( GL_ARRAY_BUFFER,
                     POSITION_OFFSET,
                     sizeof(glm::vec3)*positions->size(),
                     model->Positions()->data() );
      
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    model->setVBO(vbo->Buffer(), 0);

    //Shaders
    shared_ptr<GLShader> vertex(new GLShader(GL_VERTEX_SHADER, "vshader"));
    shared_ptr<GLShader> fragment(new GLShader(GL_FRAGMENT_SHADER, "fshader"));

    //Program
    shared_ptr<GLProgram> program(new GLProgram("qgl_program"));
    
    //Add Shaders
    program->AddShader(vertex);
    program->AddShader(fragment);

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
    projection->Set(glm::perspective(FOV, 
                    float(this->size().width())/float(this->size().height()),
                    SENSOR_DISTANCE, FOCAL_DISTANCE));

    //Add Program
    if( !this->AddProgram(program) )
    {
        cerr << "[F] Program failed to link."<<endl;
        qApp->quit();
        exit(0);
    }
    this->AddToContext(program);
    
    //Create UBOs
    shared_ptr<GLBufferObject> fubo( new GLBufferObject(
                                     "GColors",
                                     sizeof(glm::vec3),
                                     NUM_COLORS,
                                     COLOR_OFFSET,
                                     GL_UNIFORM_BUFFER,
                                     GL_STATIC_DRAW));


    shared_ptr<GLBufferObject> vubo( new GLBufferObject(
                                     "GMatrices",
                                     sizeof(glm::mat4),
                                     NUM_UNIFORMS,
                                     POSITION_OFFSET,
                                     GL_UNIFORM_BUFFER,
                                     GL_STREAM_DRAW));
    //Bind uniform indices
    vector<string> uvertex = {"mvpMatrix"};
    vector<string> ufragment = {"color"};

    vector<shared_ptr<GLUniform>> uniforms;
    uniforms = program->SetUniformIndex(vubo,
                                        uvertex,
                                        sizeof(glm::mat4),
                                        POSITION_OFFSET);
    uniforms[0]->setBuffer(vubo->Buffer()); 
    this->AddToContext(uniforms[0]); //<-- only one!
    uniforms.clear();
    uniforms = program->SetUniformIndex(fubo,
                                        ufragment,
                                        sizeof(glm::vec3),
                                        COLOR_OFFSET);

    uniforms[0]->setBuffer(fubo->Buffer()); 
    this->AddToContext(uniforms[0]);
    
    if( uniforms.size() == 0 )
    {
        cerr << "[F] UBO failed to bind."<<endl;
        qApp->quit();
        exit(0);
    }

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
    shared_ptr<GLUniform> vertex_shader = this->Get<GLUniform>("mvpMatrix");
    shared_ptr<GLUniform> frag_shader = this->Get<GLUniform>("color");

    //Bind Position Attributes to model
    glEnableVertexAttribArray(V_INDEX);
    glBindBuffer(GL_ARRAY_BUFFER, model->VBO(0));
    glVertexAttribPointer( V_INDEX,
                           3,
                           GL_FLOAT,
                           GL_FALSE,//normalized?
                           sizeof(glm::vec3),
                           0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, vertex_shader->Buffer());
    glBufferSubData( GL_UNIFORM_BUFFER,
                     vertex_shader->Offset(),
                     vertex_shader->Size(),
                     glm::value_ptr( vp * model->Matrix()) );
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    GLint offset = 0;
    //Model Fragment Loading
    for(size_t i=0; i< model->TSize(); i++)
    {   
        vector<Triangle> triangles = model->Triangles(i);
        
        glBindBuffer(GL_UNIFORM_BUFFER, frag_shader->Buffer());
        glBufferSubData(GL_UNIFORM_BUFFER,
                        frag_shader->Offset(),
                        frag_shader->Size(),
                        glm::value_ptr( glm::normalize(model->Materials().at(i).diffuse) ));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, offset, triangles.size()*3.0);
        offset += triangles.size()*3;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(V_INDEX);

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
        view->Set( glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -1.0, 4.0)) * view->Matrix());
    }
    else if( (event->key() == Qt::Key_Down))
    {
        shared_ptr<GLTransform> view = this->Get<GLTransform>("view");
        view->Set( glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 1.0, -4.0)) * view->Matrix());
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

