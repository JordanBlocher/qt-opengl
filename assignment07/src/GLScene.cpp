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

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include <GLShader.hpp>
#include <GLProgram.hpp>
#include <GLBufferObject.hpp>
#include <GLModel.hpp>
#include <GLUniform.hpp>
#include <GLCamera.hpp>

#include "GLScene.hpp"


const GLuint NUM_ATTRIBUTES = 3;

using namespace std;

GLScene::GLScene(QWidget *parent, int argc, char* argv[]) : GLViewport(parent), background(QColor::fromRgbF(0.0, 0.0, 0.2)), font(Qt::white)
{   
    int i = 1;
    while(argv[i])
    {
        this->models.push_back(argv[i]);
        ++i;
    }
    this->setContextMenuPolicy(Qt::DefaultContextMenu);    

    std::shared_ptr<GLCamera> camera(new GLCamera("camera", this->initialSize));
    this->AddToContext(camera);


}

void GLScene::initializeGL()
{
 
    GLViewport::initializeGL();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    string filename;

    //Models
    shared_ptr<GLModel> model(new GLModel(this->models.at(0).c_str(), "model", NUM_ATTRIBUTES));

    if( model->CreateVAO() )
        this->AddToContext(model);

    for(size_t i=1; i<models.size(); i++)
    {
        shared_ptr<GLModel> child(new GLModel(this->models.at(i).c_str(), "child", NUM_ATTRIBUTES));
       model->Add(child);
       child->CreateVAO();
    }

    //Shaders
    shared_ptr<GLShader> vertex(new GLShader(GL_VERTEX_SHADER, "vshader"));
    shared_ptr<GLShader> fragment(new GLShader(GL_FRAGMENT_SHADER, "fshader"));
    shared_ptr<GLShader> tvertex(new GLShader("tvertex.glsl", GL_VERTEX_SHADER, "texvshader"));
    shared_ptr<GLShader> tfragment(new GLShader("tfragment.glsl", GL_FRAGMENT_SHADER, "texfshader"));

    //Programs
    shared_ptr<GLProgram> cprogram(new GLProgram("color_program"));
    shared_ptr<GLProgram> tprogram(new GLProgram("texture_program"));
    
    //Add Shaders
    cprogram->AddShader(vertex); 
    cprogram->AddShader(fragment); 
    tprogram->AddShader(tvertex); 
    tprogram->AddShader(tfragment); 

    //Add Program
    if( this->AddProgram(cprogram) )
        this->AddToContext( cprogram );
    if( this->AddProgram(tprogram) )
        this->AddToContext( tprogram );
    
    //Create UBOs 
    std::shared_ptr<GLUniform> vertex_uniform(new GLUniform("GMatrices"));
    vertex_uniform->CreateUBO(1, cprogram->getId(), POSITION, GL_STATIC_DRAW);
    this->AddToContext(vertex_uniform);
    
    std::shared_ptr<GLUniform> frag_uniform(new GLUniform("GColors"));
    frag_uniform->CreateUBO(1, cprogram->getId(), COLOR, GL_STREAM_DRAW);
    this->AddToContext(frag_uniform);

    //Add Sampler
    std::shared_ptr<GLUniform> texture_uniform(new GLUniform("Texture", tprogram->getId(), 1, "i"));
    this->AddToContext(texture_uniform);

    //Set UBOs t Share
    cprogram->SetUBO(vertex_uniform);
    cprogram->SetUBO(frag_uniform);
    tprogram->SetUBO(vertex_uniform);

    //this->SetScene(perspective);

}

void GLScene::paintGL()
{
    //Clear the screen
    qreal r, g, b;
    this->background.getRgbF(&r, &g, &b);
    glClearColor(r, g, b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    //Get view & projection matrices
    shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera");
    glm::mat4 vp = camera->Projection() * camera->View();
    
    //Choose Model
    shared_ptr<GLModel> model = this->Get<GLModel>("model");
    
    //Get UBOS
    shared_ptr<GLUniform> vuniform = this->Get<GLUniform>("GMatrices");
    shared_ptr<GLUniform> cuniform = this->Get<GLUniform>("GColors");
    
    //Get Programs
    shared_ptr<GLProgram> tprogram = this->Get<GLProgram>("texture_program");
    shared_ptr<GLProgram> cprogram = this->Get<GLProgram>("color_program");

    //Bind MVP
    Uniform position = vuniform->Get(POSITION);
    glEnableVertexAttribArray(V_INDEX);
    glBindBuffer(GL_UNIFORM_BUFFER, vuniform->getId());
    glBufferSubData( GL_UNIFORM_BUFFER,
                     position.offset,
                     position.size,
                     glm::value_ptr( vp * model->Matrix() ));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //Get Sampler
    shared_ptr<GLUniform> tuniform = this->Get<GLUniform>("Texture");

    //Colors Program
    glUseProgram(cprogram->getId());
    model->Draw(cuniform, cprogram->getId());
    glUseProgram(0);

    //Texture Program
    glUseProgram(tprogram->getId());
    model->Draw(tuniform, tprogram->getId());
    glUseProgram(0);
}

void GLScene::idleGL()
{
    GLViewport::updateGL();
}

void GLScene::resizeGL(int width, int height)
{
    GLViewport::resizeGL(width, height);
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
    shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera");
    shared_ptr<GLModel> model = this->Get<GLModel>("model");

    GLViewport::keyPressEvent(event);

      if( (event->key() == Qt::Key_Right))
    {
        camera->SetView(glm::rotate( camera->View() * glm::mat4(1.0f), 15.0f, glm::vec3(0.0, 1.0, 0.0)) );
    }
    else if( (event->key() == Qt::Key_Left))
    {
        camera->SetView(glm::rotate( camera->View()  * glm::mat4(1.0f), -15.0f, glm::vec3(0.0, 1.0, 0.0)) );
    }
    else if( (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_Up)
    {
        camera->SetView( glm::scale(glm::mat4(1.0f), glm::vec3(1.25, 1.25, 1.25)) * model->Matrix());
    }
    else if( (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_Down)
    {
        camera->SetView( glm::scale(glm::mat4(1.0f), glm::vec3(1.0/1.25, 1.0/1.25, 1.0/1.25)) * model->Matrix());
    }
    else if( (event->key() == Qt::Key_Up))
    {
        camera->SetView( glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -1.0, 4.0)) * camera->View());
    }
    else if( (event->key() == Qt::Key_Down))
    {
        camera->SetView( glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 1.0, -4.0)) * camera->View());
    }
        GLViewport::keyPressEvent(event);
}

void GLScene::mousePressEvent(QMouseEvent *event)
{
    GLViewport::mousePressEvent(event);
}

void GLScene::contextMenuEvent(QContextMenuEvent *event)
{
    GLViewport::contextMenuEvent(event);
}


