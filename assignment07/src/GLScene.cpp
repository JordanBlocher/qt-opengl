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

GLScene::GLScene(QWidget *parent, int argc, char* argv[]) : GLViewport(parent), background(QColor::fromRgbF(0.0, 0.0, 0.2)), font(Qt::white)
{   

    this->dynamicModels = {"puck.obj", "sphere.obj","cube.obj",};
    this->staticModels = {"walls.obj"};
    this->setContextMenuPolicy(Qt::DefaultContextMenu);   
    // Initialize Entity list
    entities = std::shared_ptr<std::vector<std::shared_ptr<Entity>>>(new std::vector<std::shared_ptr<Entity>>);
}

void GLScene::initializeGL()
{

    GLViewport::initializeGL();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    string filename;

   // Initialize Dynamics World
    std::shared_ptr<DynamicsWorld> world(new DynamicsWorld("dynamics"));
    this->AddToContext(world);

    /****** Model Creation ******/
    /* -------------------Table------------------- */
    // Create the table gfxmodel
    std::shared_ptr<GLModel> tableGfx(new GLModel("walls.obj", "table", NUM_ATTRIBUTES));
    tableGfx->CreateVAO();
    tableGfx->setMatrix(glm::scale(tableGfx->Matrix(), glm::vec3(1.0f))); 

    // Create the table physmodel
    std::shared_ptr<PhysicsModel> tablePhys(new PhysicsModel("table",tableGfx,btVector3(1.0f,1.0f,1.0f),
                                            btQuaternion(0,0,0,1), btVector3(0,0,0),
                                            0.0f, 0.5f, 1.0f));

    // Add table to world 
    world->AddPhysicsBody(tablePhys->GetRigidBody());

    // Special table settings
    tablePhys->GetRigidBody()->setCollisionFlags(tablePhys->GetRigidBody()->getCollisionFlags()|btCollisionObject::CF_KINEMATIC_OBJECT);
    tablePhys->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);

    // Merge models, add to entity list
    std::shared_ptr<Entity> tableEnt(new Entity(tableGfx,tablePhys));
    entities->push_back(tableEnt);

    /* -------------------Puck------------------- */
    // Create the puck gfxmodel
    std::shared_ptr<GLModel> puckGfx(new GLModel("puck.obj", "puck", NUM_ATTRIBUTES));
    puckGfx->CreateVAO();
    puckGfx->setMatrix(glm::scale(puckGfx->Matrix(), glm::vec3(0.2f))); 

    // Create the puck physmodel
    std::shared_ptr<PhysicsModel> puckPhys(new PhysicsModel("puck",PhysicsModel::BODY::CYLINDER, btVector3(0.2f,0.1f,0.2f),
                                            btQuaternion(0,0,0,1), btVector3(1.0f,0.25f,0.0f),
                                            0.7f, 0.05f, 0.5f));

    // Add puck to world 
    world->AddPhysicsBody(puckPhys->GetRigidBody());

    // Set up puck constraints
    puckPhys->initConstraints(btVector3(0.0f,0.01f,0.0f),btVector3(1.0f,1.0f,1.0f),btVector3(0.0f,0.0f,0.0f),
                    btVector3(0.0f,0.0f,1.0f),btVector3(0.0f,0.0f,0.0f));

    // Add constraints to world
    world->AddConstraint(puckPhys->GetConstraint());

    // Special sphere settings
    puckPhys->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);

    // Merge models, add to entity list
    std::shared_ptr<Entity> puckEnt(new Entity(puckGfx, puckPhys));
    entities->push_back(puckEnt);

    /* -------------------Sphere------------------- */
    // Create the sphere gfxmodel
    std::shared_ptr<GLModel> sphereGfx(new GLModel("sphere.obj", "sphere", NUM_ATTRIBUTES));
    sphereGfx->CreateVAO();
    sphereGfx->setMatrix(glm::scale(sphereGfx->Matrix(), glm::vec3(0.2f))); 

    // Create the sphere physmodel
    std::shared_ptr<PhysicsModel> spherePhys(new PhysicsModel("sphere",PhysicsModel::BODY::SPHERE, btVector3(0.25f,0.0f,0.0f),
                                            btQuaternion(0,0,0,1), btVector3(0,0.25,0),
                                            0.7f, 0.05f, 0.5f));

    // Add sphere to world 
    world->AddPhysicsBody(spherePhys->GetRigidBody());

    // Set up sphere constraints
    spherePhys->initConstraints(btVector3(0.0f,0.01f,0.0f),btVector3(1.0f,1.0f,1.0f),btVector3(0.0f,0.0f,0.0f),
                    btVector3(1.0f,1.0f,1.0f),btVector3(0.0f,0.0f,0.0f));

    // Add constraints to world
    world->AddConstraint(spherePhys->GetConstraint());

    // Special sphere settings
    spherePhys->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);

    // Merge models, add to entity list
    std::shared_ptr<Entity> sphereEnt(new Entity(sphereGfx, spherePhys));
    entities->push_back(sphereEnt);

    /* -------------------Cube------------------- */
    // Create the cube gfxmodel
    /*std::shared_ptr<GLModel> cubeGfx(new GLModel("cube.obj", "cube", NUM_ATTRIBUTES));
    cubeGfx->CreateVAO();
    cubeGfx->setMatrix(glm::scale(cubeGfx->Matrix(), glm::vec3(0.5f))); 


    // Create the cube physmodel
    std::shared_ptr<PhysicsModel> cubePhys(new PhysicsModel("cube",PhysicsModel::BODY::BOX,
                                            btVector3(0.25f,0.25f,0.25f),btQuaternion(0,0,0,1),
                                            btVector3(0,0,0), 0.0f, 0.05f, 1.0f));

    // Add cube to world 
    world->AddPhysicsBody(cubePhys->GetRigidBody());

    // Special cube settings
    cubePhys->GetRigidBody()->setCollisionFlags(cubePhys->GetRigidBody()->getCollisionFlags()|btCollisionObject::CF_KINEMATIC_OBJECT);
    cubePhys->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);

    // Merge models, add to entity list
    std::shared_ptr<Entity> cubeEnt(new Entity(cubeGfx,cubePhys));
    entities->push_back(cubeEnt);*/



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
       transform =  pmodel->GetTransform();// * pmodel->GetTranslation();
       std::cout<<"transform "<<transform<<endl;

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


void GLScene::idleGL()
{  
    // Timer
    float dt;
    time = chrono::high_resolution_clock::now();
    dt = chrono::duration_cast< std::chrono::duration<float> >(time-this->start_time).count();
    this->start_time = chrono::high_resolution_clock::now();

    // Get Discrete Dynamics World and update time step
    std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
    std::shared_ptr<btDiscreteDynamicsWorld> world = dynamics->GetWorld();
    world->stepSimulation(dt/1.0f);

    cout << "Vel: " << entities->at(0)->getPhysicsModel()->GetRigidBody()->getLinearVelocity().length() << endl;
    btTransform newTransform;
    entities->at(0)->getPhysicsModel()->GetRigidBody()->getMotionState()->getWorldTransform(newTransform);
    cout << "Trasform:" << newTransform.getOrigin().getX() << ", " << newTransform.getOrigin().getY() << ", " << newTransform.getOrigin().getZ() << endl;

    // Update all of the fuck
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
            entities->at(1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,1)*50);
            break;
        case (Qt::Key_S):
            entities->at(1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,-1)*50);
            break;
        case (Qt::Key_A):
            entities->at(1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(1,0,0)*50);
            break;
        case (Qt::Key_D):
            entities->at(1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(-1,0,0)*50);
            break;
        case (Qt::Key_I):
            entities->at(2)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,1)*50);
            break;
        case (Qt::Key_K):
            entities->at(2)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,-1)*50);
            break;
        case (Qt::Key_J):
            entities->at(2)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(1,0,0)*50);
            break;
        case (Qt::Key_L):
            entities->at(2)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(-1,0,0)*50);
            break;
    }


}

void GLScene::mousePressEvent(QMouseEvent *event)
{
    GLViewport::mousePressEvent(event);
}

void GLScene::contextMenuEvent(QContextMenuEvent *event)
{
    GLViewport::contextMenuEvent(event);
}


