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
#include <glm/virtrev/xstream.hpp>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include <GLShader.hpp>
#include <GLProgram.hpp>
#include <GLBufferObject.hpp>
#include <GLModel.hpp>
#include <GLUniform.hpp>
#include <GLCamera.hpp>
#include <PhysicsModel.hpp>
#include <DynamicsWorld.hpp>
#include <Entity.hpp>

#include "GLScene.hpp"

const GLuint NUM_ATTRIBUTES = 3;

using namespace std;

GLScene::GLScene(int width, int height, QWidget *parent, int argc, char* argv[]) : GLViewport(width, height, parent, NULL), background(QColor::fromRgbF(0.0, 0.0, 0.2)), font(Qt::white)
{   
    this->puckTypes = {"Puck.obj"};
    this->paddleTypes = {"Paddle.obj", "squarePaddle.obj"};
    this->setContextMenuPolicy(Qt::DefaultContextMenu);   
    this->update = true;
    this->puckIndex = 1;
}

void GLScene::playGame(int numPlayers)
{
    this->numPlayers = numPlayers;
    std::shared_ptr<GLCamera> camera2(new GLCamera("camera2", this->initialSize));
    this->AddToContext(camera2);
}

void GLScene::changePaddle(int i)
{
    std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
    std::unique_ptr<btDiscreteDynamicsWorld> world = std::move(dynamics->GetWorld());

    dynamics->SetWorld(std::move(world));
}


void GLScene::initializeGL()
{
   GLViewport::initializeGL();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    this->initGame();
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
    
    //Get UBOS
    shared_ptr<GLUniform> vuniform = this->Get<GLUniform>("GMatrices");
    shared_ptr<GLUniform> cuniform = this->Get<GLUniform>("GColors");
    
    //Get Programs
    shared_ptr<GLProgram> tprogram = this->Get<GLProgram>("texture_program");
    shared_ptr<GLProgram> cprogram = this->Get<GLProgram>("color_program");

    // Iterate and draw over all of the models
    for(int i=0; i<entities->size(); i++)
    {
       //Choose Model
       std::shared_ptr<PhysicsModel> pmodel = entities->at(i)->getPhysicsModel();
       std::shared_ptr<GLModel> gmodel = entities->at(i)->getGraphicsModel();
       glm::mat4 transform;
       transform =  pmodel->GetTransform();

       //Bind MVP
       Uniform position = vuniform->Get(POSITION);
       glEnableVertexAttribArray(V_INDEX);
       glBindBuffer(GL_UNIFORM_BUFFER, vuniform->getId());
       glBufferSubData( GL_UNIFORM_BUFFER,
                        position.offset,
                        position.size,
                        glm::value_ptr( vp * transform * gmodel->Matrix()));
       glBindBuffer(GL_UNIFORM_BUFFER, 0);

       //Get Sampler
       shared_ptr<GLUniform> tuniform = this->Get<GLUniform>("Texture");

       //Colors Program
       glUseProgram(cprogram->getId());
       gmodel->Draw(cuniform, cprogram->getId());
       glUseProgram(0);

       //Texture Program
       glUseProgram(tprogram->getId());
       gmodel->Draw(tuniform, tprogram->getId());
       glUseProgram(0);
      
   }

}

void GLScene::initGame()
{
    // Initialize Entity list
    entities = std::shared_ptr<std::vector<std::shared_ptr<Entity>>>(new std::vector<std::shared_ptr<Entity>>);

    // Initialize Dynamics World
    std::shared_ptr<DynamicsWorld> world(new DynamicsWorld("dynamics"));
    this->AddToContext(world);

    btQuaternion orientation = btQuaternion(0, 0, 0, 1);
    btVector3 ones = btVector3(1.0f, 1.0f, 1.0f);
    btVector3 zeros = btVector3(0.0f, 0.0f, 0.0f);

    /****** STATIC ******/
    // Table Model
    std::shared_ptr<GLModel> tableGfx(new GLModel("table.obj", "table", NUM_ATTRIBUTES));
    tableGfx->CreateVAO();
    tableGfx->setMatrix(glm::scale(tableGfx->Matrix(), glm::vec3(1.0f))); 
    // Table Constraints
    std::shared_ptr<GLModel> tableConst(new GLModel("walls.obj", "table", NUM_ATTRIBUTES));
    tableConst->LoadVertexData();
    std::shared_ptr<PhysicsModel> tablePhys(new PhysicsModel("table",tableConst, ones, orientation, zeros, 0.0f, 0.0f, 0.5f, PhysicsModel::COLLISION::STATIC));
    world->AddPhysicsBody(tablePhys->GetRigidBody());
    // Table entity
    std::shared_ptr<Entity> tableEnt(new Entity(tableGfx,tablePhys));
    entities->push_back(tableEnt);

    /****** DYNAMIC (PADDLE) ******/
    for(int i=0; i<paddleTypes.size(); i++)
    {
        //Create Models 
        std::shared_ptr<GLModel> paddleGfx(new GLModel(paddleTypes[i].c_str(), "paddle", NUM_ATTRIBUTES));
        paddleGfx->CreateVAO();
        paddleGfx->setMatrix(glm::scale(paddleGfx->Matrix(), glm::vec3(0.2f))); 

        // Create Collision Objects
        std::shared_ptr<PhysicsModel> paddle1Phys(new PhysicsModel("paddle", paddleGfx, btVector3(0.2f,0.01f,0.2f), orientation, btVector3(0.0f,0.0f,-3.0f), 0.7f, 0.00f, 0.5f, PhysicsModel::COLLISION::DYNAMIC));
        std::shared_ptr<PhysicsModel> paddle2Phys(new PhysicsModel("paddle", paddleGfx, btVector3(0.2f,0.01f,0.2f), orientation, btVector3(0.0f,0.0f,-3.0f), 0.7f, 0.0f, 0.5f, PhysicsModel::COLLISION::DYNAMIC));
        
        // Initialize World
        // Add rigid body and then constraint
        world->AddPhysicsBody(paddle1Phys->GetRigidBody());
        paddle1Phys->initConstraints(zeros, ones, zeros, btVector3(0.0f,1.0f,0.0f), zeros);
        world->AddConstraint(paddle1Phys->GetConstraint());
        world->AddPhysicsBody(paddle2Phys->GetRigidBody());
        paddle2Phys->initConstraints(zeros, ones, zeros, btVector3(0.0f,1.0f,0.0f), zeros);
        world->AddConstraint(paddle2Phys->GetConstraint());

        // Set Restrictive Dynamics Constraints
        paddle1Phys->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
        paddle1Phys->GetRigidBody()->setLinearFactor(btVector3(1,0,1));
        paddle1Phys->GetRigidBody()->setAngularFactor(btVector3(0,1,0));
        paddle2Phys->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
        paddle2Phys->GetRigidBody()->setLinearFactor(btVector3(1,0,1));
        paddle2Phys->GetRigidBody()->setAngularFactor(btVector3(0,1,0));

        // Merge models to entity list
        std::shared_ptr<Entity> paddleEnt1(new Entity(paddleGfx, paddle1Phys));
        entities->push_back(paddleEnt1);
        std::shared_ptr<Entity> paddleEnt2(new Entity(paddleGfx, paddle2Phys));
        entities->push_back(paddleEnt2);
    }
    
    /****** DYNAMIC (PUCK) ******/
    for(int i=0; i<puckTypes.size(); i++)
    {
        //Create Models 
        std::shared_ptr<GLModel> puckGfx(new GLModel(puckTypes[i].c_str(), "puck", NUM_ATTRIBUTES));
        puckGfx->CreateVAO();
        puckGfx->setMatrix(glm::scale(puckGfx->Matrix(), glm::vec3(0.2f))); 

        // Create Collision Objects
        std::shared_ptr<PhysicsModel> puckPhys(new PhysicsModel("puck",PhysicsModel::BODY::CYLINDER, btVector3(0.2f,0.01f,0.2f), orientation, btVector3(0.0f,0.0f,-3.0f), 0.7f, 0.00f, 0.5f));
        // Initialize World
        // Add rigid body and then constraint
        world->AddPhysicsBody(puckPhys->GetRigidBody());
        puckPhys->initConstraints(zeros, ones, zeros, btVector3(0.0f,1.0f,0.0f), zeros);
        world->AddConstraint(puckPhys->GetConstraint());

        // Set Restrictive Dynamics Constraints
        puckPhys->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
        puckPhys->GetRigidBody()->setLinearFactor(btVector3(1,0,1));
        puckPhys->GetRigidBody()->setAngularFactor(btVector3(0,1,0));

        // Merge models to entity list
        std::shared_ptr<Entity> puckEnt(new Entity(puckGfx, puckPhys));
        entities->push_back(puckEnt);
    }

}

void GLScene::idleGL()
{  
    if( update )
    {
        // Timer
        float dt;
        time = chrono::high_resolution_clock::now();
        dt = chrono::duration_cast< std::chrono::duration<float> >(time-this->start_time).count();
        this->start_time = chrono::high_resolution_clock::now();

        // Get Discrete Dynamics World and update time step
        std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
        std::unique_ptr<btDiscreteDynamicsWorld> world = std::move(dynamics->GetWorld());
        world->stepSimulation(dt/1.0f);
        dynamics->SetWorld(std::move(world));

        // Update all of the poo
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
    shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera");
    //shared_ptr<GLCamera> camera2 = this->Get<GLCamera>("camera2");

    // Let the superclass handle the events
    GLViewport::keyPressEvent(event);

    // Act on the key press event
    switch(event->key())
    {
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
       case (Qt::Key_W):
            entities->at(this->puckIndex)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,1)*50);
            break;
        case (Qt::Key_S):
            entities->at(this->puckIndex)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,-1)*50);
            break;
        case (Qt::Key_A):
            entities->at(this->puckIndex)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(1,0,0)*50);
            break;
        case (Qt::Key_D):
            entities->at(this->puckIndex)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(-1,0,0)*50);
            break;
        case (Qt::Key_I):
            entities->at(puckIndex+1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,1)*50);
            break;
        case (Qt::Key_K):
            entities->at(puckIndex+1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,-1)*50);
            break;
        case (Qt::Key_J):
            entities->at(puckIndex+1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(1,0,0)*50);
            break;
        case (Qt::Key_L):
            entities->at(puckIndex+1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(-1,0,0)*50);
            break;
        case (Qt::Key_Space):
            this->stop();
            emit mainMenu(1);
            break;
    }


}

std::shared_ptr<std::vector<std::shared_ptr<Entity>>> GLScene::getEntities()
{
    return this->entities;
}

void GLScene::mousePressEvent(QMouseEvent *event)
{
    GLViewport::mousePressEvent(event);
}

void GLScene::contextMenuEvent(QContextMenuEvent *event)
{
    GLViewport::contextMenuEvent(event);
}

void GLScene::resume()
{
    if(!this->update)
    {
        GLViewport::start_time = std::chrono::high_resolution_clock::now();
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

GLScene::~GLScene()
{
    std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
    std::unique_ptr<btDiscreteDynamicsWorld> world = std::move(dynamics->GetWorld());
    world.reset();
    dynamics->SetWorld(std::move(world));
}


