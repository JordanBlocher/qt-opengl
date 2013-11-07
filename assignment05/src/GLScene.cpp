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
#include <GLModel.hpp>
#include <GLUniform.hpp>
#include <GLCamera.hpp>
#include <GLEmissive.hpp>

#include "GLScene.hpp"

#define FOV 45.0f
#define SENSOR_DISTANCE 0.01f
#define FOCAL_DISTANCE 100.0f

const GLuint NUM_ATTRIBUTES = 3;

using namespace std;

GLScene::GLScene(int width, int height, QWidget *parent, int argc, char* argv[]) : GLViewport(width, height, parent, NULL), update(true), background(QColor::fromRgbF(0.0, 0.0, 0.2)), font(Qt::white)
{
    this->modelpath = argv[1];
    if(argc > 2)
        this->materialpath = argv[2];
    this->setContextMenuPolicy(Qt::DefaultContextMenu);    
}

void GLScene::initializeGL()
{
    GLViewport::initializeGL();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    //Models
    shared_ptr<GLModel> model(new GLModel(this->modelpath.c_str(), "model", NUM_ATTRIBUTES));

    if( model->CreateVAO() )
        this->AddToContext(model);

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
    vertex_uniform->CreateUBO(cprogram->getId(), 1, GL_STATIC_DRAW);
    this->AddToContext(vertex_uniform);
    
    std::shared_ptr<GLUniform> frag_uniform(new GLUniform("GColors"));
    frag_uniform->CreateUBO(cprogram->getId(), 2, GL_STREAM_DRAW);
    this->AddToContext(frag_uniform);

    std::shared_ptr<GLUniform> lights_uniform(new GLUniform("GLights"));
    lights_uniform->CreateUBO(cprogram->getId(), 3, GL_STREAM_DRAW);
    this->AddToContext(lights_uniform);

    //Add Sampler
    std::shared_ptr<GLUniform> texture_uniform(new GLUniform("Texture", tprogram->getId(), 1, "i"));
    this->AddToContext(texture_uniform);

    //Set UBOs t Share
    cprogram->SetUBO(vertex_uniform);
    cprogram->SetUBO(frag_uniform);
    tprogram->SetUBO(vertex_uniform);
   
    //Set Lighting
    std::shared_ptr<GLEmissive> lighting(new GLEmissive("lights"));
    lighting->ambient.color = glm::vec3(1.0f, 1.0f, 1.0f);
    lighting->ambient.intensity = 0.4f;
    this->AddToContext(lighting);


}

void GLScene::paintGL()
{
    //Clear the screen
    qreal r, g, b;
    this->background.getRgbF(&r, &g, &b);
    glClearColor(r, g, b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    //Get view & projection matrices
    shared_ptr<GLCamera> camera1 = this->Get<GLCamera>("camera1");
    glm::mat4 vp = camera1->Projection() * camera1->View();
    
    //Choose Model
    shared_ptr<GLModel> model = this->Get<GLModel>("model");
    
    //Get UBOS
    shared_ptr<GLUniform> vuniform = this->Get<GLUniform>("GMatrices");
    shared_ptr<GLUniform> cuniform = this->Get<GLUniform>("GColors");
    shared_ptr<GLUniform> luniform = this->Get<GLUniform>("GLights");
    
    //Get Programs
    shared_ptr<GLProgram> tprogram = this->Get<GLProgram>("texture_program");
    shared_ptr<GLProgram> cprogram = this->Get<GLProgram>("color_program");

    //Get Lights
    shared_ptr<GLEmissive> lighting = this->Get<GLEmissive>("lights");

    //Bind MVP
    Matrices matrices;
    matrices.mvpMatrix = vp * model->Matrix();
    matrices.mvMatrix = camera1->View() *  model->Matrix();
    matrices.normalMatrix = glm::transpose(glm::inverse(camera1->View() * model->Matrix()));
    glBindBuffer(GL_UNIFORM_BUFFER, vuniform->getId());
    glBufferSubData( GL_UNIFORM_BUFFER,
                     0,
                     sizeof(matrices),
                     &matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
   
    // Bind Lights
    glBindBuffer(GL_UNIFORM_BUFFER, luniform->getId());
    glBufferSubData( GL_UNIFORM_BUFFER,
                     0,
                     sizeof(lighting->ambient),
                     &lighting->ambient);
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
    if( update )
    {
        GLViewport::updateGL();
    }
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
    shared_ptr<GLCamera> camera1 = this->Get<GLCamera>("camera1");
    
    GLViewport::keyPressEvent(event);

    // Act on the key press event
    switch(event->key())
    {
        case (Qt::Key_Right):
            // Move RIGHT
            camera1->moveCamera(GLCamera::CamDirection::Right);
            break;    
        case (Qt::Key_Left):
            // Move LEFT
            camera1->moveCamera(GLCamera::CamDirection::Left);
            break;
        case (Qt::Key_Up):
            // Forward if SHIFT, UP otherwise
            if(event->modifiers() & Qt::ShiftModifier){
                camera1->moveCamera(GLCamera::CamDirection::Forward);
            }
            else
            {
                camera1->moveCamera(GLCamera::CamDirection::Up);
            }
            break;
        case (Qt::Key_Down):
            // Backward if SHIFT, DOWN otherwise
            if(event->modifiers() & Qt::ShiftModifier){
                camera1->moveCamera(GLCamera::CamDirection::Backward);
            }
            else
            {
                camera1->moveCamera(GLCamera::CamDirection::Down);
            }
            break;            
    }


}

void GLScene::mousePressEvent(QMouseEvent *event)
{
   event->accept();
}

void GLScene::contextMenuEvent(QContextMenuEvent *event)
{
}

void GLScene::resume()
{
    if(!this->update)
    {
        GLViewport::start_time  = std::chrono::high_resolution_clock::now();
        this->update = true;
        this->time = std::chrono::high_resolution_clock::now();
        GLViewport::timer.start();
    }
}

void GLScene::pause()
{
    if(this->update)
    {
        this->update = false;
        GLViewport::timer.stop();
    }
}

void GLScene::playGame(int)
{
}

void GLScene::changePaddle(int)
{
}
