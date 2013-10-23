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
    this->staticModels = {"table.obj"};
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

    // Create Dynamic Models
    //for(size_t i=0; i<1; i++)
    {
        std::shared_ptr<GLModel> tempGfxModel1(new GLModel(this->dynamicModels.at(0).c_str(), "dynamicModel", NUM_ATTRIBUTES));
        std::shared_ptr<GLModel> tempGfxModel2(new GLModel(this->dynamicModels.at(1).c_str(), "dynamicModel", NUM_ATTRIBUTES));
        std::shared_ptr<GLModel> tempGfxModel3(new GLModel(this->dynamicModels.at(2).c_str(), "dynamicModel", NUM_ATTRIBUTES));


        // Create the VAO for the new Graphics Model
        tempGfxModel1->CreateVAO();
        tempGfxModel1->setMatrix(glm::scale(tempGfxModel1->Matrix(), glm::vec3(0.2))); 

        tempGfxModel2->CreateVAO();
        tempGfxModel2->setMatrix(glm::scale(tempGfxModel2->Matrix(), glm::vec3(0.2))); 

        tempGfxModel3->CreateVAO();      
        tempGfxModel3->setMatrix(glm::scale(tempGfxModel3->Matrix(), glm::vec3(0.2))); 

        // Create 2 dynamic Models
        std::shared_ptr<PhysicsModel> tempPhysModel1(new PhysicsModel("dynamicBody", 0.7f, 0.5f,0.05f, glm::vec3(0.2f,0.2f,0.2f), glm::vec3(0.1f,0.1f,0.1f), PhysicsModel::BODY::CYLINDER));
        tempPhysModel1->SetTransform(glm::vec4(0, 0, 0, 1), glm::vec3(0, 0, 0));
        tempPhysModel1->SetConstraints(glm::vec3(0.0,0.0,0.0),glm::vec3(1,0,1),glm::vec3(0,0,0),glm::vec3(0,1,0),glm::vec3(0,0,0));
        //tempPhysModel1->SetMotionState(glm::vec3(2,0,5));
        world->AddPhysicsBody(tempPhysModel1->GetRigidBody(), tempPhysModel1->GetConstraint());
        std::shared_ptr<Entity> ent1(new Entity(tempGfxModel1,tempPhysModel1));
        // Add the new Ent to the vector
        entities->push_back(ent1);
        std::shared_ptr<PhysicsModel> tempPhysModel2(new PhysicsModel("dynamicBody", 0.7f, 0.5f,0.05f, glm::vec3(0.2f,0.2f,0.2f), glm::vec3(0.1f,0.1f,0.1f), PhysicsModel::BODY::SPHERE));
        tempPhysModel2->SetTransform(glm::vec4(0, 0, 0, 1), glm::vec3(0, 0, 0));
        tempPhysModel2->SetConstraints(glm::vec3(0.0,0.0,0.0),glm::vec3(1,0,1),glm::vec3(0,0,0),glm::vec3(0,1,0),glm::vec3(0,0,0));
                //tempPhysModel2->SetMotionState(glm::vec3(3,0,0));

        world->AddPhysicsBody(tempPhysModel2->GetRigidBody(), tempPhysModel2->GetConstraint());
        std::shared_ptr<Entity> ent2(new Entity(tempGfxModel2,tempPhysModel2));
        entities->push_back(ent2);

        // Place Static Objects
        std::shared_ptr<PhysicsModel> tempPhysModel3(new PhysicsModel("dynamicBody", 0.0f, 0.5f,0.05f, glm::vec3(0.2f,0.2f,0.2f), glm::vec3(0.05f,0.05f,0.05f), PhysicsModel::BODY::BOX));
        tempPhysModel3->SetTransform(glm::vec4(0, 0, 0, 1), glm::vec3(0, 0, 0));
        tempPhysModel3->SetConstraints(glm::vec3(0.0,0.0,0.0),glm::vec3(1,0,1),glm::vec3(0,0,0),glm::vec3(0,1,0),glm::vec3(0,0,0));
        tempPhysModel3->SetMotionState(glm::vec3(0,0,2));
        world->AddPhysicsBody(tempPhysModel3->GetRigidBody(), tempPhysModel3->GetConstraint());
        std::shared_ptr<Entity> ent3(new Entity(tempGfxModel3,tempPhysModel3));
        entities->push_back(ent3);

  /*      std::shared_ptr<PhysicsModel> tempPhysModel4(new PhysicsModel("dynamicBody", 0.0f, 0.5f,0.05f, glm::vec3(0.2,0.2,6), glm::vec3(0.05f,0.05f,0.05f), PhysicsModel::BODY::BOX));
        tempPhysModel4->SetTransform(glm::vec4(0, 0, 0, 1), glm::vec3(0, 0, 0));
        tempPhysModel4->SetConstraints(glm::vec3(0.0,0.0,0.0),glm::vec3(1,0,1),glm::vec3(0,0,0),glm::vec3(0,1,0),glm::vec3(0,0,0));
        tempPhysModel4->SetMotionState(glm::vec3(5,0,2));
        world->AddPhysicsBody(tempPhysModel4->GetRigidBody(), tempPhysModel4->GetConstraint());

        std::shared_ptr<PhysicsModel> tempPhysModel5(new PhysicsModel("dynamicBody", 0.0f, 0.5f,0.05f, glm::vec3(0.2,0.2,6), glm::vec3(0.05f,0.05f,0.05f), PhysicsModel::BODY::BOX));
        tempPhysModel5->SetTransform(glm::vec4(0, 0, 0, 1), glm::vec3(0, 0, 0));
        tempPhysModel5->SetConstraints(glm::vec3(0.0,0.0,0.0),glm::vec3(1,0,1),glm::vec3(0,0,0),glm::vec3(0,1,0),glm::vec3(0,0,0));
        tempPhysModel5->SetMotionState(glm::vec3(-5,0,2));
        world->AddPhysicsBody(tempPhysModel5->GetRigidBody(), tempPhysModel5->GetConstraint());
*/

    }

    // Create static Models
    for(size_t i=0; i<1; i++)
    {
        std::shared_ptr<GLModel> staticModel(new GLModel("table.obj", "staticModel", NUM_ATTRIBUTES));
        staticModel->setMatrix(glm::scale(staticModel->Matrix(), glm::vec3(3, 0.1, 3))); 

        // Create the VAO for the new Graphics Model
        staticModel->CreateVAO();
        // Create entity
        std::shared_ptr<PhysicsModel> staticPhysModel(new PhysicsModel("table", staticModel->RigidBody(), staticModel->RigidBody()->size()));
        //std::shared_ptr<PhysicsModel> staticPhysModel(new PhysicsModel("dynamicBody", 0.0f, 0.5f,0.05f, glm::vec3(0.2,0.2,0.2), glm::vec3(0.05f,0.05f,0.05f), PhysicsModel::BODY::BOX));
        //staticPhysModel->SetTransform(glm::vec4(0, 0, 0, 1), glm::vec3(0, 0, 0));
        staticPhysModel->SetConstraints(glm::vec3(0.0,0.0,0.0),glm::vec3(1,0,1),glm::vec3(0,0,0),glm::vec3(0,1,0),glm::vec3(0,0,0));
        //staticPhysModel->SetMotionState(glm::vec3(0,0,0));

        staticPhysModel->GetRigidBody()->setCollisionFlags(staticPhysModel->GetRigidBody()->getCollisionFlags()|btCollisionObject::CF_KINEMATIC_OBJECT);
        staticPhysModel->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
        // WILL BE RE-ENABLED VERY SOON, ENLARGING THE MESH. 
        world->AddPhysicsBody(staticPhysModel->GetRigidBody(), staticPhysModel->GetConstraint());

        std::shared_ptr<Entity> ent10(new Entity(staticModel,staticPhysModel));

        entities->push_back(ent10);
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
       glm::mat4 transform = pmodel->GetTransform();
       //std::cout<<"transform "<<transform<<endl;

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
    world->stepSimulation(dt);

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
            entities->at(0)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,1)*50);
            break;
        case (Qt::Key_S):
            entities->at(0)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,-1)*50);
            break;
        case (Qt::Key_A):
            entities->at(0)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(1,0,0)*50);
            break;
        case (Qt::Key_D):
            entities->at(0)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(-1,0,0)*50);
            break;
        case (Qt::Key_I):
            entities->at(1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,1)*50);
            break;
        case (Qt::Key_K):
            entities->at(1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(0,0,-1)*50);
            break;
        case (Qt::Key_J):
            entities->at(1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(1,0,0)*50);
            break;
        case (Qt::Key_L):
            entities->at(1)->getPhysicsModel()->GetRigidBody()->applyCentralForce(btVector3(-1,0,0)*50);
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


