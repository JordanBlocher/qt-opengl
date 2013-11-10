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
#include <QWheelEvent>

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

GLScene::GLScene(int width, int height, QWidget *parent, int argc, char* argv[]) : GLViewport(width, height, parent, NULL), background(QColor::fromRgbF(0.0, 0.0, 0.2)), font(Qt::white)
{
    this->setContextMenuPolicy(Qt::DefaultContextMenu);  

    for(int index=0; index < 12; index++)
        this->keyHeld[index] = false;

    // Start the timer
    this->time = std::chrono::high_resolution_clock::now();
    GLViewport::timer.start();  
}

void GLScene::initializeGL()
{
   GLViewport::initializeGL();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

       /****** Deep GPU Stuff ******/
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
    shared_ptr<GLUniform> vertex_uniform(new GLUniform("GMatrices"));
    vertex_uniform->CreateUBO(cprogram->getId(), 1, GL_STATIC_DRAW);
    this->AddToContext(vertex_uniform);
    
    shared_ptr<GLUniform> frag_uniform(new GLUniform("GColors"));
    frag_uniform->CreateUBO(cprogram->getId(), 2, GL_STREAM_DRAW);
    this->AddToContext(frag_uniform);

    shared_ptr<GLUniform> lights_uniform(new GLUniform("GLights"));
    lights_uniform->CreateUBO(cprogram->getId(), 3, GL_STREAM_DRAW);
    this->AddToContext(lights_uniform);

    shared_ptr<GLUniform> eye_uniform(new GLUniform("Eye"));
    eye_uniform->CreateUBO(cprogram->getId(), 4, GL_STREAM_DRAW);
    this->AddToContext(eye_uniform);

    //Add Sampler
    shared_ptr<GLUniform> texture_uniform(new GLUniform("Texture", tprogram->getId(), 1, "i"));
    this->AddToContext(texture_uniform);

    //Set UBOs t Share
    cprogram->SetUBO(vertex_uniform);
    cprogram->SetUBO(lights_uniform);
    cprogram->SetUBO(frag_uniform);
    cprogram->SetUBO(eye_uniform);
    tprogram->SetUBO(vertex_uniform);
    tprogram->SetUBO(lights_uniform);
    tprogram->SetUBO(eye_uniform);

    //Set Lighting
    shared_ptr<GLEmissive> emissive(new GLEmissive("lights"));
    this->AddToContext(emissive);
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
    if( this->Contains("model") )
    {
        shared_ptr<GLModel> model = this->Get<GLModel>("model");
         
        //Get UBOS
        shared_ptr<GLUniform> vuniform = this->Get<GLUniform>("GMatrices");
        shared_ptr<GLUniform> cuniform = this->Get<GLUniform>("GColors");
        shared_ptr<GLUniform> luniform = this->Get<GLUniform>("GLights");
        shared_ptr<GLUniform> eye = this->Get<GLUniform>("Eye");
        
        //Get Programs
        shared_ptr<GLProgram> tprogram = this->Get<GLProgram>("texture_program");
        shared_ptr<GLProgram> cprogram = this->Get<GLProgram>("color_program");

        //Get Lights
        shared_ptr<GLEmissive> emissive = this->Get<GLEmissive>("lights");

        //Bind MVP
        Matrices matrices;
        matrices.mvpMatrix = vp * model->Matrix();
        matrices.mvMatrix = model->Matrix();
        matrices.normalMatrix = glm::transpose(glm::inverse(model->Matrix()));
        glBindBuffer(GL_UNIFORM_BUFFER, vuniform->getId());
        glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(matrices), &matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
       
        // Eye Position
        glBindBuffer(GL_UNIFORM_BUFFER, eye->getId());
        glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(glm::vec4(camera1->getCameraPosition(), 1.0f)));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Bind Lights
        glBindBuffer(GL_UNIFORM_BUFFER, luniform->getId());
        size_t baseSize = sizeof(emissive->lights.basic);
        size_t ptSize = sizeof(emissive->lights.point);
        glBufferSubData( GL_UNIFORM_BUFFER, 0, baseSize, &emissive->lights.basic);
        glBufferSubData( GL_UNIFORM_BUFFER, baseSize + 8, sizeof(emissive->lights.point[0]), &emissive->lights.point[0]);
        glBufferSubData( GL_UNIFORM_BUFFER, baseSize + ptSize + 16, sizeof(emissive->lights.spot[0]), &emissive->lights.spot[0]);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        //Get Sampler
        shared_ptr<GLUniform> tuniform = this->Get<GLUniform>("Texture");

        //Colors Program
        glUseProgram(cprogram->getId());
        model->Draw(cuniform, cprogram->getId());
        glUseProgram(0);
        
        //Texture Program
       // glUseProgram(tprogram->getId());
       // model->Draw(tuniform, tprogram->getId());
       // glUseProgram(0);
    }
          

}

void GLScene::idleGL()
{
    // Timer
    float dt;
    time = chrono::high_resolution_clock::now();
    dt = chrono::duration_cast< std::chrono::duration<float> >(time-this->start_time).count();
    this->start_time = chrono::high_resolution_clock::now();

    GLViewport::updateGL();
    updateKeys();

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
    // Let the superclass handle the events
    GLViewport::keyPressEvent(event);
    
    shared_ptr<GLEmissive> emissive = this->Get<GLEmissive>("lights");
    // Act on the key press event
    switch(event->key())
    {
        case (Qt::Key_Right):
            keyHeld[8] = true;
            break;    
        case (Qt::Key_Left):
            keyHeld[9] = true;
            break;
        case (Qt::Key_Up):
            keyHeld[10] = true;
            break;
        case (Qt::Key_Down):
            keyHeld[11] = true;
            break;            
        case (Qt::Key_Escape):
            emit mainMenu(0);
            break;
        case(Qt::Key_1):
            emissive->lights.basic.base.ambientIntensity +=0.05f;
            break;
        case(Qt::Key_2):
            emissive->lights.basic.base.ambientIntensity -=0.05f;
            break;
        case(Qt::Key_3):
            emissive->lights.basic.base.diffuseIntensity +=0.05f;
            break;
        case(Qt::Key_4):
            emissive->lights.basic.base.diffuseIntensity -=0.05f;
            break;

    }
}

void GLScene::keyReleaseEvent(QKeyEvent *event)
{
    // Act on the key press event
    switch(event->key())
    {
        case (Qt::Key_Right):
            keyHeld[8] = false;
            break;    
        case (Qt::Key_Left):
            keyHeld[9] = false;
            break;
        case (Qt::Key_Up):
            keyHeld[10] = false;
            break;
        case (Qt::Key_Down):
            keyHeld[11] = false;
            break;            
    }
}

void GLScene::wheelEvent(QWheelEvent *event)
{
    shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera1");

    if(event->delta() > 0) // RG
    {
        camera->moveCamera(GLCamera::CamDirection::Backward);
    }
    if(event->delta() < 0) // LF
    {
        camera->moveCamera(GLCamera::CamDirection::Forward);
    }

}

void GLScene::updateKeys()
{
    shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera1");

    if(keyHeld[8]) // RG
    {
        camera->moveCamera(GLCamera::CamDirection::Right);
    }
    if(keyHeld[9]) // LF
    {
        camera->moveCamera(GLCamera::CamDirection::Left);
    }
    if(keyHeld[10]) // UP
    {
        camera->moveCamera(GLCamera::CamDirection::Up);
    }
    if(keyHeld[11]) // DW
    {
        camera->moveCamera(GLCamera::CamDirection::Down);
    }  
}

void GLScene::setModel(const char* name)
{
    this->RemoveFromContext("model");
    shared_ptr<GLModel> model(new GLModel(name, "model", NUM_ATTRIBUTES));
    if( model->CreateVAO() )
        this->AddToContext(model);
}

void GLScene::mousePressEvent(QMouseEvent *event)
{
   event->accept();
}

void GLScene::contextMenuEvent(QContextMenuEvent *event)
{
    GLViewport::contextMenuEvent(event);
}


