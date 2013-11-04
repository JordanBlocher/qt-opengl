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
    this->paddleTypes = {"Paddle.obj", "squarePaddle.obj", "trianglePaddle.obj"};
    this->setContextMenuPolicy(Qt::DefaultContextMenu);   
    this->update = true;
    this->puckIndex = 7;
    this->paddleIndex = 1;
    for(int index=0; index < 12; index++)
        this->keyHeld[index] = false;
}

void GLScene::playGame(int numPlayers)
{
    this->puckIndex = 7;
    this->paddleIndex = 1;
    this->numPlayers = numPlayers;       
    shared_ptr<GLCamera> camera1 = this->Get<GLCamera>("camera1");
    camera1->SetView(18.0f, M_PI, 0.4f*M_PI);

    this->player1Score = 0;
    this->player2Score = 0;

    if(numPlayers > 1)
    {
        std::shared_ptr<GLCamera> camera2(new GLCamera("camera2", this->initialSize));
        camera2->SetView(18.0f, 2.0*M_PI, 0.40f*M_PI);
        camera1->SetProjection(glm::perspective(45.0f, float(this->width()/2.0f)/float(this->height()), 0.01f, 100.0f)); 
        camera2->SetProjection(glm::perspective(45.0f, float(this->width()/2.0f)/float(this->height()), 0.01f, 100.0f)); 
        this->AddToContext(camera2);
    }
    else
    {
        GLViewport::resizeGL(this->width(), this->height());
    }
    this->removeDynamicBodies();
    this->addDynamicBodies();
    this->update = false;
    this->resume();
}

void GLScene::changePaddle(int i)
{
    std::shared_ptr<btRigidBody> paddle1Old = this->entities->at(paddleIndex)->getPhysicsModel()->GetRigidBody();
    std::shared_ptr<btRigidBody> paddle2Old = this->entities->at(paddleIndex + 1)->getPhysicsModel()->GetRigidBody();

    paddle1Old->setCollisionFlags( paddle1Old->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    paddle2Old->setCollisionFlags( paddle2Old->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

    paddle1Old->setLinearVelocity(btVector3(0,0,0));
    paddle1Old->setAngularVelocity(btVector3(0,0,0));
    paddle2Old->setLinearVelocity(btVector3(0,0,0));
    paddle2Old->setAngularVelocity(btVector3(0,0,0));

    this->paddleIndex = 2*i + 1;

    std::shared_ptr<btRigidBody> paddle1 = this->entities->at(paddleIndex)->getPhysicsModel()->GetRigidBody();
    //paddle1->translate(paddle1Old->getCenterOfMassPosition());
    std::shared_ptr<btRigidBody> paddle2 = this->entities->at(paddleIndex + 1)->getPhysicsModel()->GetRigidBody();
    //paddle2->translate(paddle2Old->getCenterOfMassPosition());

    paddle1->setCollisionFlags( paddle1->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
    paddle2->setCollisionFlags( paddle2->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);

    this->entities->at(paddleIndex)->getPhysicsModel()->SetPosition(btVector3(-3,0,0));
    this->entities->at(paddleIndex+1)->getPhysicsModel()->SetPosition(btVector3(3,0,0));

}


void GLScene::initializeGL()
{
   GLViewport::initializeGL();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    this->initGame();
    this->addStaticBodies();
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
 
    for(int i=0; i<numPlayers; i++)
    {
        //Get view & projection matrices
        shared_ptr<GLCamera> camera = this->Get<GLCamera>(string("camera" + to_string(i+1)).c_str() );
        glm::mat4 vp = camera->Projection() * camera->View();

        glViewport(i * this->width()/(1.0*numPlayers), 0.0, this->width()/(1.0*numPlayers), this->height()); 

        //Get UBOS
        shared_ptr<GLUniform> vuniform = this->Get<GLUniform>("GMatrices");
        shared_ptr<GLUniform> cuniform = this->Get<GLUniform>("GColors");
        
        //Get Programs
        shared_ptr<GLProgram> tprogram = this->Get<GLProgram>("texture_program");
        shared_ptr<GLProgram> cprogram = this->Get<GLProgram>("color_program");

        // Iterate and draw over all of the models
        vector<int> indices = {0, puckIndex, paddleIndex, paddleIndex+1};
        for(int i=0; i<indices.size(); i++)
        {
           int index = indices[i];
           //Choose Model
           std::shared_ptr<PhysicsModel> pmodel = entities->at(index)->getPhysicsModel();
           std::shared_ptr<GLModel> gmodel = entities->at(index)->getGraphicsModel();
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

}

void GLScene::initGame()
{
    // Initialize Entity list
    entities = std::shared_ptr<std::vector<std::shared_ptr<Entity>>>(new std::vector<std::shared_ptr<Entity>>);

    // Initialize Dynamics World
    std::shared_ptr<DynamicsWorld> world(new DynamicsWorld("dynamics"));
    this->AddToContext(world);

}

void GLScene::addStaticBodies()
{
    std::shared_ptr<DynamicsWorld> world = this->Get<DynamicsWorld>("dynamics");

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
    std::shared_ptr<PhysicsModel> tablePhys(new PhysicsModel("table",tableConst, ones, orientation, zeros, 0.0f, 1.0f, 0.5f, PhysicsModel::COLLISION::STATIC));
    world->AddPhysicsBody(tablePhys->GetRigidBody());
    // Table entity
    std::shared_ptr<Entity> tableEnt(new Entity(tableGfx,tablePhys));
    entities->push_back(tableEnt);

}

void GLScene::addDynamicBodies()
{
    std::shared_ptr<DynamicsWorld> world = this->Get<DynamicsWorld>("dynamics");

    btQuaternion orientation = btQuaternion(0, 0, 0, 1);
    btVector3 ones = btVector3(1.0f, 1.0f, 1.0f);
    btVector3 zeros = btVector3(0.0f, 0.0f, 0.0f);


    /****** DYNAMIC (PADDLE) ******/
    for(int i=0; i<paddleTypes.size(); i++)
    {
        //Create Models 
        std::shared_ptr<GLModel> paddleGfx(new GLModel(paddleTypes[i].c_str(), "paddle", NUM_ATTRIBUTES));
        paddleGfx->CreateVAO();
        paddleGfx->setMatrix(glm::scale(paddleGfx->Matrix(), glm::vec3(0.3f))); 
        // Create Collision Objects
        std::shared_ptr<PhysicsModel> paddle1Phys(new PhysicsModel("paddle", paddleGfx, btVector3(0.2f,0.01f,0.2f), orientation, btVector3(-3.0f,0.0f,0.0f), 0.7f, 5000.0f, 0.0f, PhysicsModel::COLLISION::DYNAMIC));
        std::shared_ptr<PhysicsModel> paddle2Phys(new PhysicsModel("paddle", paddleGfx, btVector3(0.2f,0.01f,0.2f), orientation, btVector3(3.0f,0.0f,0.0f), 0.7f, 5000.0f, 0.0f, PhysicsModel::COLLISION::DYNAMIC));
        
        // Initialize World
        // Add rigid body and then constraint
        world->AddPhysicsBody(paddle1Phys->GetRigidBody());
        paddle1Phys->initConstraints(zeros, btVector3(-3.6,1,1), btVector3(3.0,0,0), btVector3(0.0f,1.0f,0.0f), zeros);
        world->AddConstraint(paddle1Phys->GetConstraint());
        world->AddPhysicsBody(paddle2Phys->GetRigidBody());
        paddle2Phys->initConstraints(zeros, btVector3(-3,1,1), btVector3(3.6,0,0), btVector3(0.0f,1.0f,0.0f), zeros);
        world->AddConstraint(paddle2Phys->GetConstraint());

        // Set Restrictive Dynamics Constraints
        paddle1Phys->GetRigidBody()->setLinearFactor(btVector3(1,0,1));
        paddle1Phys->GetRigidBody()->setAngularFactor(btVector3(0,1,0));
        paddle2Phys->GetRigidBody()->setLinearFactor(btVector3(1,0,1));
        paddle2Phys->GetRigidBody()->setAngularFactor(btVector3(0,1,0));

        paddle1Phys->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
        paddle2Phys->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);


        if(i > 0)
        {
            paddle1Phys->GetRigidBody()->setCollisionFlags( paddle1Phys->GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
            paddle2Phys->GetRigidBody()->setCollisionFlags( paddle2Phys->GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        else
        {
            paddle1Phys->GetRigidBody()->setCollisionFlags( paddle1Phys->GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
            paddle2Phys->GetRigidBody()->setCollisionFlags( paddle2Phys->GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    

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
        puckGfx->setMatrix(glm::scale(puckGfx->Matrix(), glm::vec3(0.3f))); 

        // Create Collision Objects
        std::shared_ptr<PhysicsModel> puckPhys(new PhysicsModel("puck",PhysicsModel::BODY::CYLINDER, btVector3(0.2f,0.01f,0.2f), orientation, btVector3(0.0f,0.0f,0.0f), 0.01f, 0.00f, 0.5f));

        // Initialize World
        // Add rigid body and then constraint
        world->AddPhysicsBody(puckPhys->GetRigidBody());
        puckPhys->initConstraints(zeros, btVector3(-7.5,1,-7.5), btVector3(7.5,0,7.5), btVector3(0.0f,1.0f,0.0f), zeros);
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


void GLScene::removeDynamicBodies()
{
    std::shared_ptr<DynamicsWorld> world = this->Get<DynamicsWorld>("dynamics");

    size_t numEnts = entities->size();
    if( numEnts == 0 )
        return;
    for(size_t i=numEnts - 1; i>0; i--)
    {
        std::shared_ptr<Entity> ent = entities->at(i);
        entities->pop_back();
        world->RemoveDynamicPhysics(ent->getPhysicsModel());
    }
}

void GLScene::idleGL()
{  
    if( update )
    {
        bool newRound = false;
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

        // Update all of the physics dependent keys
        updateKeys();

        // Print the position of the puck
        btVector3 puckPos = entities->at(this->puckIndex)->getPhysicsModel()->GetRigidBody()->getCenterOfMassPosition();
        if(puckPos.getX() > 7.0f)
        {
            player1Score++;
            emit updateScore(player1Score,1);
            newRound = true;
        }
        else if(puckPos.getX() < -7.0f)
        {
            player2Score++;
            emit updateScore(player2Score,2);
            newRound = true;
        }

        if (newRound)
        {
            // Reset the puck position
            this->entities->at(this->puckIndex)->getPhysicsModel()->SetPosition(btVector3(0,0,0));
            this->entities->at(this->puckIndex)->getPhysicsModel()->GetRigidBody()->setLinearVelocity(btVector3(0,0,0));
            this->entities->at(this->puckIndex)->getPhysicsModel()->GetRigidBody()->setAngularVelocity(btVector3(0,0,0));

            // Reset the paddle positions
            this->entities->at(this->paddleIndex)->getPhysicsModel()->SetPosition(btVector3(-3,0,0));
            this->entities->at(this->paddleIndex+1)->getPhysicsModel()->SetPosition(btVector3(3,0,0));
            this->entities->at(this->paddleIndex)->getPhysicsModel()->GetRigidBody()->setLinearVelocity(btVector3(0,0,0));
            this->entities->at(this->paddleIndex+1)->getPhysicsModel()->GetRigidBody()->setLinearVelocity(btVector3(0,0,0));
            this->entities->at(this->paddleIndex)->getPhysicsModel()->GetRigidBody()->setAngularVelocity(btVector3(0,0,0));
            this->entities->at(this->paddleIndex+1)->getPhysicsModel()->GetRigidBody()->setAngularVelocity(btVector3(0,0,0));

        }
    }
}

void GLScene::resizeGL(int width, int height)
{
    if(numPlayers > 1)
    {
        shared_ptr<GLCamera> camera1 = this->Get<GLCamera>("camera1");
        camera1->SetProjection(glm::perspective(45.0f, float(width/2.0)/float(height), 0.01f, 100.0f)); 
        shared_ptr<GLCamera> camera2 = this->Get<GLCamera>("camera2");
        camera2->SetProjection(glm::perspective(45.0f, float(width/2.0)/float(height), 0.01f, 100.0f)); 
    }
    else
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
    
    if(!event->isAutoRepeat())
    {
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
            case (Qt::Key_W):
                keyHeld[0] = true;
                break;
            case (Qt::Key_S):
                keyHeld[1] = true;
                break;
            case (Qt::Key_A):
                keyHeld[2] = true;
                break;
            case (Qt::Key_D):
                keyHeld[3] = true;
                break;
            case (Qt::Key_I):
                keyHeld[4] = true;
                break;
            case (Qt::Key_K):
                keyHeld[5] = true;
                break;
            case (Qt::Key_J):
                keyHeld[6] = true;
                break;
            case (Qt::Key_L):
                keyHeld[7] = true;
                break;
            case (Qt::Key_Escape):
                if(this->update)
                {
                    this->pause();
                }
                else
                {
                    this->resume();
                }
                emit mainMenu(1);
                break;
            case (Qt::Key_Q):
                emit endGame();
                break;
        }
    }

}

void GLScene::keyReleaseEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
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
            case (Qt::Key_W):
                keyHeld[0] = false;
                break;
            case (Qt::Key_S):
                keyHeld[1] = false;
                break;
            case (Qt::Key_A):
                keyHeld[2] = false;
                break;
            case (Qt::Key_D):
                keyHeld[3] = false;
                break;
            case (Qt::Key_I):
                keyHeld[4] = false;
                break;
            case (Qt::Key_K):
                keyHeld[5] = false;
                break;
            case (Qt::Key_J):
                keyHeld[6] = false;
                break;
            case (Qt::Key_L):
                keyHeld[7] = false;
                break;
        }
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

void GLScene::updateKeys()
{
    shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera1");

    if(keyHeld[0]) // W
        entities->at(this->paddleIndex)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(1,0,0)*40);
    if(keyHeld[1]) // S
        entities->at(this->paddleIndex)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(-1,0,0)*40);
    if(keyHeld[2]) // A
        entities->at(this->paddleIndex)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,-1)*40);
    if(keyHeld[3]) // D
        entities->at(this->paddleIndex)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,1)*40);
    if(keyHeld[4]) // I
        entities->at(paddleIndex+1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(-1,0,0)*40);
    if(keyHeld[5]) // K
        entities->at(paddleIndex+1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(1,0,0)*40);
    if(keyHeld[6]) // J
        entities->at(paddleIndex+1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,1)*40);
    if(keyHeld[7]) // L
        entities->at(paddleIndex+1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,-1)*40);


    entities->at(paddleIndex)->getPhysicsModel()->GetRigidBody()->setLinearVelocity(0.92f * entities->at(paddleIndex)->getPhysicsModel()->GetRigidBody()->getLinearVelocity());
    entities->at(paddleIndex+1)->getPhysicsModel()->GetRigidBody()->setLinearVelocity(0.92f * entities->at(paddleIndex+1)->getPhysicsModel()->GetRigidBody()->getLinearVelocity());

    if(numPlayers > 1)
    {
        shared_ptr<GLCamera> camera2 = this->Get<GLCamera>("camera2");
        if(keyHeld[8]) // RG
        {
            camera->moveCamera(GLCamera::CamDirection::Right);
            camera2->moveCamera(GLCamera::CamDirection::Right);
        }
        if(keyHeld[9]) // LF
        {
            camera->moveCamera(GLCamera::CamDirection::Left);
            camera2->moveCamera(GLCamera::CamDirection::Left);
        }
        if(keyHeld[10]) // UP
        {
            camera->moveCamera(GLCamera::CamDirection::Up);
            camera2->moveCamera(GLCamera::CamDirection::Up);
        }
        if(keyHeld[11]) // DW
        {
            camera->moveCamera(GLCamera::CamDirection::Down);
            camera2->moveCamera(GLCamera::CamDirection::Down);
        }
    }
    else
    {
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
}

GLScene::~GLScene()
{
    std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
    std::unique_ptr<btDiscreteDynamicsWorld> world = std::move(dynamics->GetWorld());
    world.reset();
    dynamics->SetWorld(std::move(world));
}


