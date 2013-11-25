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
#include <glm/virtrev/xstream.hpp>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include <GLShader.hpp>
#include <GLProgram.hpp>
#include <GLBufferObject.hpp>
#include <GLModel.hpp>
#include <GLUniform.hpp>
#include <GLCamera.hpp>
#include <GLEmissive.hpp>
#include <PhysicsModel.hpp>
#include <DynamicsWorld.hpp>
#include <Entity.hpp>
#include <PhysicsDebug.hpp>

#include "GLScene.hpp"

#define FOV 45.0f
#define SENSOR_DISTANCE 0.01f
#define FOCAL_DISTANCE 100.0f

const GLuint NUM_ATTRIBUTES = 3;

using namespace std;

GLScene::GLScene(int width, int height, QWidget *parent, int argc, char* argv[]) : GLViewport(width, height, parent, NULL), background(QColor::fromRgbF(0.0, 0.0, 0.2)), font(Qt::white)
{
    // Setup tcp listener thread
    tcpListen = new TcpListen(this);
    tcpListen->QThread::start();

    // Create sound manager
    std::shared_ptr<SoundManager> soundMan(new SoundManager("soundMan"));
    this->AddToContext(soundMan);

    // Play first bgm

    soundMan->emitPlayBgm(0);

    this->gameLevels = {"level1.obj","1hardmaze56.obj"};
    this->balls = {"greensphere.obj","redsphere.obj"};
    this->startPosition = { glm::vec3(2.5f, 1.0f, 2.5f), glm::vec3(4.8f, 1.0f, -4.0f) };
    this->endPosition = { glm::vec3(-2.4,-1.6,-2.3),glm::vec3(4.8,-2,4.8) };
    this->levelCount = 2;

    this->levelIdx = 0;
    this->setContextMenuPolicy(Qt::DefaultContextMenu);  

    for(int index=0; index < 12; index++)
        this->keyHeld[index] = false;

    keyHeld[5] = keyHeld[6] = keyHeld[7] = true; 

    this->update = true;
    this->damping = true;
        gameTime = 0;


    connect(this, SIGNAL(dataRcvd(glm::vec3)), this, SLOT(dataWorker(glm::vec3)));
}

void GLScene::initGame()
{
    // Initialize Entity list
    entities = std::shared_ptr<std::vector<std::shared_ptr<Entity>>>(new std::vector<std::shared_ptr<Entity>>);

    // Initialize Dynamics World
    std::shared_ptr<DynamicsWorld> world(new DynamicsWorld("dynamics"));
    this->AddToContext(world);

    // Add dynamic bodies to World
    // Create entity (scale, mass, friction, restitution, centroid)

    for(int i = 0; i < this->levelCount; i ++)
    {
        // Board
        std::shared_ptr<Entity> board(new Entity(0.1, 0.0f, 0.1f, 1.0f, btVector3(0, 0, 0)));
        board->Create(this->gameLevels[i].c_str(), NULL, Entity::BODY::STATIC);
        world->AddPhysicsBody(board->GetPhysicsModel()->GetRigidBody());
        board->Constrain(Entity::DYNAMIC_NO_G);
        world->AddConstraint(board->GetPhysicsModel()->GetConstraint());
        this->entities->push_back(board);

        // Ball
        std::shared_ptr<Entity> ball(new Entity(0.25, 0.5f, 0.0f, 1.0f, btVector3(0, 0, 0)));
        ball->Create(this->balls[i].c_str(), NULL, Entity::BODY::SPHERE);
        world->AddPhysicsBody(ball->GetPhysicsModel()->GetRigidBody());
        ball->Constrain(Entity::DYNAMIC);
        world->AddConstraint(ball->GetPhysicsModel()->GetConstraint());
        this->entities->push_back(ball);
    }

    // Initialize the controls
    this->tablePitch = 0.0f;
    this->tableRoll = 0.0f;
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
    cout<<"Matrix UBO"<<endl;
    shared_ptr<GLUniform> vertex_uniform(new GLUniform("GMatrices"));
    vertex_uniform->CreateUBO(cprogram->getId(), 1, GL_STATIC_DRAW);
    this->AddToContext(vertex_uniform);
    
    cout<<"Color UBO"<<endl;
    shared_ptr<GLUniform> frag_uniform(new GLUniform("GColors"));
    frag_uniform->CreateUBO(cprogram->getId(), 2, GL_STREAM_DRAW);
    this->AddToContext(frag_uniform);
    cout<<"Colors index " <<glGetUniformBlockIndex(cprogram->getId(), "GColors")<<endl;

    cout<<"Lights UBO"<<endl;
    shared_ptr<GLUniform> lights_uniform(new GLUniform("GLights"));
    lights_uniform->CreateUBO(cprogram->getId(), 3, GL_STREAM_DRAW);
    this->AddToContext(lights_uniform);

    cout<<"Eye UBO"<<endl;
    shared_ptr<GLUniform> eye_uniform(new GLUniform("Eye"));
    eye_uniform->CreateUBO(cprogram->getId(), 4, GL_STREAM_DRAW);
    this->AddToContext(eye_uniform);

    //Add Sampler
    shared_ptr<GLUniform> texture_uniform(new GLUniform("Texture", tprogram->getId(), 1, "i"));
    this->AddToContext(texture_uniform);

    //Set UBOs to Share
    cprogram->SetUBO(vertex_uniform);
    cprogram->SetUBO(lights_uniform);
    cprogram->SetUBO(frag_uniform);
    cprogram->SetUBO(eye_uniform);
    tprogram->SetUBO(vertex_uniform);
    tprogram->SetUBO(lights_uniform);
    tprogram->SetUBO(eye_uniform);
    tprogram->SetUBO(frag_uniform);

    //Set Lighting
    shared_ptr<GLEmissive> emissive(new GLEmissive("lights"));
    this->AddToContext(emissive);

    #ifdef DEBUG_DRAWING
    this->DebugDrawInit();
    #endif 

    #ifdef PHYSICS_DEBUG
    this->DebugPhysicsInit();
    #endif
}

void GLScene::playGame(int level)
{
    // Get the nodes that we need
    shared_ptr<GLCamera> camera1 = this->Get<GLCamera>("camera1");
    std::shared_ptr<SoundManager> soundMan = this->Get<SoundManager>("soundMan");  
    std::shared_ptr<Entity> ball = this->entities->at((level*2)+1);
    std::shared_ptr<Entity> maze = this->entities->at(level*2);

    // Hack
    this->levelIdx = level;

    // Set up the view
    camera1->SetView(10.0f,0.0f,0.0f);

    // Disable all of the balls and tables that we don't want
    for(int i = 0; i < levelCount*2; i++)
    {
        std::shared_ptr<btRigidBody> gameEnt = entities->at(i)->GetPhysicsModel()->GetRigidBody();
        gameEnt->setCollisionFlags( gameEnt->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        gameEnt->setLinearVelocity(btVector3(0,0,0));
        gameEnt->setAngularVelocity(btVector3(0,0,0));
    }

    // Enable the tables and balls that we do want
    ball->GetPhysicsModel()->GetRigidBody()->setCollisionFlags(ball->GetPhysicsModel()->GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
    maze->GetPhysicsModel()->GetRigidBody()->setCollisionFlags(maze->GetPhysicsModel()->GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);

    // Place the ball at the correct location
    btScalar x = this->startPosition[level].x;
    btScalar y = this->startPosition[level].y;
    btScalar z = this->startPosition[level].z;
    ball->GetPhysicsModel()->SetPosition(btVector3(x, y, z));

    // Ensure that the game starts
    this->update = false;
    this->resume();

    // Set up the damn clock thing
    elapsedTime = 0.0f;
    gameTime = 0;

    soundMan->emitPlayBgm(levelIdx+1);

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
    
    for(int i=0; i<2; i++)
    {
        // Get Dynamic Models
        std::shared_ptr<PhysicsModel> pmodel = entities->at(i+levelIdx*2)->GetPhysicsModel();
        std::shared_ptr<GLModel> gmodel = entities->at(i+levelIdx*2)->GetGraphicsModel();
        glm::mat4 transform = pmodel->GetTransform();
        btVector3 pos = pmodel->GetRigidBody()->getCenterOfMassPosition();

        // Get UBOS
        shared_ptr<GLUniform> vuniform = this->Get<GLUniform>("GMatrices");
        shared_ptr<GLUniform> cuniform = this->Get<GLUniform>("GColors");
        shared_ptr<GLUniform> luniform = this->Get<GLUniform>("GLights");
        shared_ptr<GLUniform> eye = this->Get<GLUniform>("Eye");
        
        // Get Programs
        shared_ptr<GLProgram> tprogram = this->Get<GLProgram>("texture_program");
        shared_ptr<GLProgram> cprogram = this->Get<GLProgram>("color_program");

        // Get Lights
        shared_ptr<GLEmissive> emissive = this->Get<GLEmissive>("lights");

        // Bind MVP
        Matrices matrices;
        glm::mat4 rot = camera1->RotMat();
        matrices.mvpMatrix = vp * transform * gmodel->Matrix();
        matrices.mvMatrix = camera1->View() * transform * gmodel->Matrix();
        matrices.normalMatrix = glm::transpose(glm::inverse(matrices.mvMatrix));
        glBindBuffer(GL_UNIFORM_BUFFER, vuniform->getId());
        glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(matrices), &matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Bind Eye Position & toggle
        glBindBuffer(GL_UNIFORM_BUFFER, eye->getId());
        glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(glm::vec4(camera1->getCameraPosition(), 1.0f)));
        glBufferSubData( GL_UNIFORM_BUFFER, sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(glm::vec4(keyHeld[4], keyHeld[5], keyHeld[6], keyHeld[7])));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Bind Lights
        glBindBuffer(GL_UNIFORM_BUFFER, luniform->getId());
        size_t baseSize = sizeof(emissive->lights.basic);
        size_t ptSize = sizeof(emissive->lights.point[0]);
        size_t sptSize = sizeof(emissive->lights.spot[0]); 
        glBufferSubData( GL_UNIFORM_BUFFER, 0, baseSize, &emissive->lights.basic);
        //cout<<"Point  offset "<<baseSize + 8<<endl;
        glBufferSubData( GL_UNIFORM_BUFFER, baseSize + 8, ptSize, &emissive->lights.point[0]);
        for(int j=0; j<6; j++)
        {
            //cout<<"Spot "<<i<<" offset "<<baseSize + ptSize + i*sptSize + 24 + 8*(i+1)<<endl;
            glBufferSubData( GL_UNIFORM_BUFFER, baseSize + ptSize + j*sptSize + 24 + 8*(j+1), sptSize, &(emissive->lights.spot[j]));
        }
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
           
    #ifdef DEBUG_DRAWING
        this->DebugDraw();
    #endif 


    #ifdef PHYSICS_DEBUG
        this->DebugPhysicsDraw();
    #endif

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

        // Update all of the physics dependent keys
        updateKeys(dt);

        // Update the ball's gravity vector
        updateBallGravVector(dt);

        // Maintain game
        checkGameState();

        elapsedTime+=dt;

        if(elapsedTime > 1.0f)
        {
            gameTime++;
            emit forceRepaint();
            elapsedTime = 0.0f;
        }

    }
}


void GLScene::resizeGL(int width, int height)
{
    GLViewport::resizeGL(width, height);
}

void GLScene::keyPressEvent(QKeyEvent *event)
{
    // Let the superclass handle the events
    GLViewport::keyPressEvent(event);
    
    // Act on the key press event
    switch(event->key())
    {   
        case(Qt::Key_1):
            keyHeld[4] = !keyHeld[4];
            break;
        case(Qt::Key_2):
            if(event->modifiers() & Qt::KeypadModifier)
            {
                keyHeld[13] = true;  
            }
            else
            {
                keyHeld[5] = !keyHeld[5];
            }
            break;
        case(Qt::Key_3):
            keyHeld[6] = !keyHeld[6];
            break;
        case(Qt::Key_4):
            if(event->modifiers() & Qt::KeypadModifier)
            {
                keyHeld[14] = true;  
            }
            else
            {
                keyHeld[7] = !keyHeld[7];
            }
            break;
         case(Qt::Key_6):
            if(event->modifiers() & Qt::KeypadModifier)
            {
                keyHeld[15] = true;  
            }     
            break;      
        case(Qt::Key_8):
            if(event->modifiers() & Qt::KeypadModifier)
            {
                keyHeld[12] = true;  
            }
            break;
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
        case(Qt::Key_W):
            keyHeld[0] = true;
            break;
        case(Qt::Key_S):
            keyHeld[1] = true;
            break;
        case(Qt::Key_A):
            keyHeld[2] = true;
            break;
        case(Qt::Key_D):
            keyHeld[3] = true;
            break;

    }
}

void GLScene::keyReleaseEvent(QKeyEvent *event)
{
    // Act on the key press event
    switch(event->key())
    {
        case(Qt::Key_2):
            if(event->modifiers() & Qt::KeypadModifier)
            {
                keyHeld[13] = false;  
            }
            break;
        case(Qt::Key_4):
            if(event->modifiers() & Qt::KeypadModifier)
            {
                keyHeld[14] = false;  
            }
            break;
        case(Qt::Key_6):
            if(event->modifiers() & Qt::KeypadModifier)
            {
                keyHeld[15] = false;  
            }     
            break;      
        case(Qt::Key_8):
            if(event->modifiers() & Qt::KeypadModifier)
            {
                keyHeld[12] = false;  
            }
            break;
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
         case(Qt::Key_W):
            keyHeld[0] = false;
            break;
        case(Qt::Key_S):
            keyHeld[1] = false;
            break;
        case(Qt::Key_A):
            keyHeld[2] = false;
            break;
        case(Qt::Key_D):
            keyHeld[3] = false;
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

void GLScene::updateKeys(float dt)
{
    shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera1");
    shared_ptr<GLEmissive> emissive = this->Get<GLEmissive>("lights");

    if(keyHeld[2]) // A
    {
        if(tablePitch+(dt*(M_PI/3.0f)) > M_PI/2.0f)
        {
            tablePitch = M_PI/2.0f;
        }
        else
        {
            tablePitch += dt*(M_PI/3.0f);
        }
    }
    if(keyHeld[3]) // D
    {
        if(tablePitch-(dt*(M_PI/3.0f)) < -M_PI/2.0f)
        {
            tablePitch = -M_PI/2.0f;
        }
        else
        {
            tablePitch -= dt*(M_PI/3.0f);
        }
    }
    if(keyHeld[0]) // W
    {
        if(tableRoll+(dt*(M_PI/3.0f)) > M_PI/2.0f)
        {
            tableRoll = M_PI/2.0f;
        }
        else
        {
            tableRoll += dt*(M_PI/3.0f);
        }
    }
    if(keyHeld[1]) // S
    {
        if(tableRoll-(dt*(M_PI/3.0f)) < -M_PI/2.0f)
        {
            tableRoll = -M_PI/2.0f;
        }
        else
        {
            tableRoll -= dt*(M_PI/3.0f);
        }
    }   
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
    if(keyHeld[12])
    {
        emissive->lights.basic.base.ambientIntensity +=0.05f;
    }
    if(keyHeld[13])
    {
        emissive->lights.basic.base.ambientIntensity -=0.05f;
    }
    if(keyHeld[14])
    {
        emissive->lights.basic.base.diffuseIntensity +=0.05f;
    }
    if(keyHeld[15])
    {
        emissive->lights.basic.base.diffuseIntensity -=0.05f;
    }
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

void GLScene::mousePressEvent(QMouseEvent *event)
{
   event->accept();
}

void GLScene::contextMenuEvent(QContextMenuEvent *event)
{
    GLViewport::contextMenuEvent(event);
}

void GLScene::updateBallGravVector(float dt)
{
    shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera1");

    // TODO: Apply this to all balls
    std::shared_ptr<Entity> ball = this->entities->at((levelIdx*2)+1);

    // Compose a rotation matrix for the table
    btMatrix3x3 xMat = btMatrix3x3(1,0,0,
            0,cos(tablePitch),-sin(tablePitch),
            0,sin(tablePitch),cos(tablePitch));
    btMatrix3x3 zMat = btMatrix3x3(cos(tableRoll),-sin(tableRoll),0,
            sin(tableRoll),cos(tableRoll),0,
            0,0,1);
    btMatrix3x3 rotMat = zMat*xMat;


    // TODO: Send the table rotation data to the camera (OPTIMIZE)
    camera->setCameraOffset(tableRoll/2.0f, tablePitch/2.0f);

    // Pass thhe ball pos into the camera
    btVector3 btBallPos = ball->GetPhysicsModel()->GetRigidBody()->getCenterOfMassPosition();
    glm::vec3 glmBallPos = glm::vec3(btBallPos.x(),btBallPos.y(),btBallPos.z());
    camera->setAimTarget(glmBallPos);

    btVector3 gravityVector = rotMat*(btVector3(0.0f,-950.0f,0.0f)*dt);
    ball->GetPhysicsModel()->GetRigidBody()->activate(true);
    ball->GetPhysicsModel()->GetRigidBody()->setGravity(gravityVector);


    // Damp the table values
    if (this->damping)
    {
        if(tablePitch-((dt/0.5f)*tablePitch) < M_PI/10000.0f && tablePitch-((dt/0.5f)*tablePitch) > -M_PI/10000.0f)
        {
            tablePitch = 0.0f;
        }
        else
        {
            tablePitch-=((dt/0.5f)*tablePitch);       
        }

        if(tableRoll-((dt/0.5f)*tableRoll) < M_PI/10000.0f && tableRoll-((dt/0.5f)*tableRoll) > -M_PI/10000.0f)
        {
            tableRoll = 0.0f;
        }
        else
        {
            tableRoll-=((dt/0.5f)*tableRoll);       
        }
    }

}

void GLScene::checkGameState()
{
    // Respawn the ball if it is below the threshold.
    std::shared_ptr<Entity> ball = this->entities->at((levelIdx*2)+1);

    btVector3 pos = ball->GetPhysicsModel()->GetRigidBody()->getCenterOfMassPosition();
    if(pos.y() < -1.5f)
    {

        if(abs(pos.z() - this->endPosition[levelIdx].z) < 0.6 && abs(pos.x() - this->endPosition[levelIdx].x) < 0.6)
        {
            if(levelIdx == levelCount-1)
            {
                emit endGame();
                this->pause();
                gameTime = 0;
            }
            else
            {
                playGame(levelIdx+1);
                gameTime = 0;
            }
        }
        else
        {
            gameTime = 0;
            emit updateScore(0, 0);
            if (p1.score == 0)
            {
                this->pause();
            }
        }

        btScalar x = this->startPosition[levelIdx].x;
        btScalar y = this->startPosition[levelIdx].y;
        btScalar z = this->startPosition[levelIdx].z;
        ball->GetPhysicsModel()->SetPosition(btVector3(x, y, z));
        
    }
}

GLScene::~GLScene()
{
    std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
    std::unique_ptr<btDiscreteDynamicsWorld> world = std::move(dynamics->GetWorld());
    world.reset();
    dynamics->SetWorld(std::move(world));
}

void GLScene::tcpDataRcvd(glm::vec3 orienData)
{
    emit dataRcvd(orienData);
}

void GLScene::dataWorker(glm::vec3 orienData)
{
    // Initialize vars
    float preZenith, preAzimuth;

    // Handle new orientation from phone
    std::cout << orienData.x << " " << orienData.y << " " << orienData.z << std::endl;

    shared_ptr<GLCamera> camera = this->Get<GLCamera>("camera1");

    // Normalize the accelerometer data for the camera
    if(orienData.x > 5.0f)
        preZenith = 5.0f;
    else if (orienData.x < -5.0f)
        preZenith = -5.0f;
    else preZenith = orienData.x;

    if(orienData.y > 5.0f)
        preAzimuth = 5.0f;
    else if (orienData.y < -5.0f)
        preAzimuth = -5.0f;
    else preAzimuth = orienData.y;

    tableRoll = -preZenith/5.0f * M_PI/4.0f;
    tablePitch = -preAzimuth/5.0f * M_PI/4.0f;

    // Turn off damping
    if(this->damping)
        this->damping = false;

}

void GLScene::DebugDrawInit()
{
  
    //Add little spheres where the spot light and point light are
    shared_ptr<GLModel> pointLightModel(new GLModel("./redsphere.obj", "pointLightModel", NUM_ATTRIBUTES));
    if( pointLightModel->CreateVAO() )
        this->AddToContext(pointLightModel);
    shared_ptr<GLModel> spotLightModel(new GLModel("./greensphere.obj", "spotLightModel", NUM_ATTRIBUTES));
    if( spotLightModel->CreateVAO() )
        this->AddToContext(spotLightModel);
    shared_ptr<GLModel> spotLightModel2(new GLModel("./greensphere.obj", "spotLightModel2", NUM_ATTRIBUTES));
    if( spotLightModel2->CreateVAO() )
        this->AddToContext(spotLightModel2);

}

void GLScene::DebugPhysicsInit()
{
    std::cout << "Initializing Debug Physics Program" << std::endl;
    //Shaders (draw normals)
    shared_ptr<GLShader> pvertex(new GLShader("pvertex.glsl", GL_VERTEX_SHADER, "passvshader"));
    shared_ptr<GLShader> pfragment(new GLShader("pfragment.glsl", GL_FRAGMENT_SHADER, "passfshader"));
    //Program
    shared_ptr<GLProgram> pprogram(new GLProgram("passthrough_program"));
    //Add Shaders
    pprogram->AddShader(pvertex);
    pprogram->AddShader(pfragment);
    //Add Program
    if( this->AddProgram(pprogram) )
        this->AddToContext( pprogram );
    //Share UBO
    shared_ptr<GLUniform> vuniform = this->Get<GLUniform>("GMatrices");
    pprogram->SetUBO(vuniform);
  
    //Add physics debug class
    std::shared_ptr<PhysicsDebug> physicsDebug(new PhysicsDebug("physicsDebug"));
    this->AddToContext(physicsDebug);

    //Add debug class to world
    std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
    std::unique_ptr<btDiscreteDynamicsWorld> world = std::move(dynamics->GetWorld());
    world->setDebugDrawer(physicsDebug.get());
    dynamics->SetWorld(std::move(world));
}

void GLScene::DebugDraw()
{
  //Get view & projection matrices
    shared_ptr<GLCamera> camera1 = this->Get<GLCamera>("camera1");
    glm::mat4 vp = camera1->Projection() * camera1->View();
    
    // Get UBOS
    shared_ptr<GLUniform> vuniform = this->Get<GLUniform>("GMatrices");
    shared_ptr<GLUniform> cuniform = this->Get<GLUniform>("GColors");
    shared_ptr<GLUniform> luniform = this->Get<GLUniform>("GLights");
    shared_ptr<GLUniform> eye = this->Get<GLUniform>("Eye");
    
    // Get Programs
    shared_ptr<GLProgram> tprogram = this->Get<GLProgram>("texture_program");
    shared_ptr<GLProgram> cprogram = this->Get<GLProgram>("color_program");

    // Get Lights
    shared_ptr<GLEmissive> emissive = this->Get<GLEmissive>("lights");

    //Get Sampler
    shared_ptr<GLUniform> tuniform = this->Get<GLUniform>("Texture");

    Matrices matrices;
    for(int i=0; i<2; i++)
    {
        //Draw Point light orb
        if ( keyHeld[6] )
        {
            shared_ptr<GLModel> pointLightModel = this->Get<GLModel>("pointLightModel");
            glm::vec4 v = emissive->lights.point[i].position;
            glm::mat4 pointMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(v.x, v.y, v.z)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
            pointLightModel->setMatrix(pointMatrix);

            matrices.mvpMatrix = vp * pointLightModel->Matrix();
            matrices.mvMatrix = pointLightModel->Matrix();
            matrices.normalMatrix = glm::transpose(glm::inverse(pointLightModel->Matrix()));
            glBindBuffer(GL_UNIFORM_BUFFER, vuniform->getId());
            glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(matrices), &matrices);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            //Colors Program
            glUseProgram(cprogram->getId());
            pointLightModel->Draw(cuniform, cprogram->getId());
            glUseProgram(0);
            
        }
    }
    
    for(int i=0; i<6; i++)
    {
        //Draw Spot light orb
        if ( keyHeld[7] )
        {
            shared_ptr<GLModel> spotLightModel = this->Get<GLModel>("spotLightModel");
            glm::vec4 v = emissive->lights.spot[i].point.position;
            glm::mat4 spotMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(v.x, v.y, v.z)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
            spotLightModel->setMatrix(spotMatrix);

            matrices.mvpMatrix = vp * spotLightModel->Matrix();
            matrices.mvMatrix = spotLightModel->Matrix();
            matrices.normalMatrix = glm::transpose(glm::inverse(spotLightModel->Matrix()));
            glBindBuffer(GL_UNIFORM_BUFFER, vuniform->getId());
            glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(matrices), &matrices);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            //Colors Program
            glUseProgram(cprogram->getId());
            spotLightModel->Draw(cuniform, cprogram->getId());
            glUseProgram(0);
        }
    }   
}

void GLScene::DebugPhysicsDraw()
{
  //Get view & projection matrices
    shared_ptr<GLCamera> camera1 = this->Get<GLCamera>("camera1");
    glm::mat4 vp = camera1->Projection() * camera1->View();


    // Get UBOS
    shared_ptr<GLUniform> vuniform = this->Get<GLUniform>("GMatrices");
    shared_ptr<GLUniform> cuniform = this->Get<GLUniform>("GColors");
    shared_ptr<GLUniform> luniform = this->Get<GLUniform>("GLights");
    shared_ptr<GLUniform> eye = this->Get<GLUniform>("Eye");
    
    // Get Programs
    shared_ptr<GLProgram> tprogram = this->Get<GLProgram>("texture_program");
    shared_ptr<GLProgram> cprogram = this->Get<GLProgram>("color_program");

    // Get Lights
    shared_ptr<GLEmissive> emissive = this->Get<GLEmissive>("lights");

    //Get Sampler
    shared_ptr<GLUniform> tuniform = this->Get<GLUniform>("Texture");

    Matrices matrices;
    shared_ptr<GLProgram> pprogram = this->Get<GLProgram>("passthrough_program");

    std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
    std::unique_ptr<btDiscreteDynamicsWorld> world = std::move(dynamics->GetWorld());
    std::shared_ptr<PhysicsDebug> physicsDebug = this->Get<PhysicsDebug>("physicsDebug");
    
    // set the matrices
    matrices.mvpMatrix = vp;
    matrices.mvMatrix = camera1->View();
    matrices.normalMatrix = camera1->View();
    glBindBuffer(GL_UNIFORM_BUFFER, vuniform->getId());
    glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(matrices), &matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // now draw and render the debug world
    glUseProgram(pprogram->getId());
    world->debugDrawWorld();
    physicsDebug->loadToBuffer();
    physicsDebug->draw();
    glUseProgram(0);
    
    dynamics->SetWorld(std::move(world));
}
