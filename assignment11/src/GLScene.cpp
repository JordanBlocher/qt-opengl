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
    this->gameLevels = {"level1.obj"};
    this->levelIdx = 0;

    this->setContextMenuPolicy(Qt::DefaultContextMenu);  

    for(int index=0; index < 12; index++)
        this->keyHeld[index] = false;

    keyHeld[4] = keyHeld[5] = keyHeld[6] = keyHeld[7] = true; 

   // Setup audio output for bgm
    bgmOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    bgmObject = new Phonon::MediaObject(this);
    bgmObject->setTickInterval(1000);
    Phonon::createPath(bgmObject, bgmOutput);
    bgmSource = new Phonon::MediaSource("start.mp3");

    // Setup audio outputs for sound effects
    for(int i = 0; i < 5; i++)
    {
        audioOutputs.append(new Phonon::AudioOutput(Phonon::MusicCategory, this));
        mediaObjects.append(new Phonon::MediaObject(this));
        mediaObjects[i]->setTickInterval(1000);
        Phonon::createPath(mediaObjects[i], audioOutputs[i]);
    }

    // Add sounds to list
    sources.append(Phonon::MediaSource("ding.mp3"));
    sources.append(Phonon::MediaSource("start.mp3"));
    sources.append(Phonon::MediaSource("hit.mp3"));

    // Connect signals to play sounds
    connect(this, SIGNAL(playBgm()), this, SLOT(playBgmWorker()));
    connect(this, SIGNAL(playEffect(int)), this, SLOT(playEffectWorker(int)));
    connect(bgmObject, SIGNAL(finished()), SLOT(finished()));

    this->update = true;
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
    
    // Board
    std::shared_ptr<Entity> board(new Entity(0.2, 1000.0f, 1.0f, 0.01f, btVector3(0, 0, 0)));
    board->Create(this->gameLevels[levelIdx].c_str(), NULL, Entity::BODY::DYNAMIC);
    world->AddPhysicsBody(board->GetPhysicsModel()->GetRigidBody());
    board->Constrain(Entity::DYNAMIC_NO_G);
    world->AddConstraint(board->GetPhysicsModel()->GetConstraint());
    this->entities->push_back(board);

    // Ball
    std::shared_ptr<Entity> ball(new Entity(0.2, 10.0f, 1.0f, 0.01f, btVector3(0, 2, 2)));
    ball->Create("ball.obj", NULL, Entity::BODY::SPHERE);
    world->AddPhysicsBody(ball->GetPhysicsModel()->GetRigidBody());
    ball->Constrain(Entity::DYNAMIC);
    world->AddConstraint(ball->GetPhysicsModel()->GetConstraint());
    this->entities->push_back(ball);
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

#ifdef PHYSICS_DEBUG
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
    pprogram->SetUBO(vertex_uniform);
  
    //Add physics debug class
    std::shared_ptr<PhysicsDebug> physicsDebug(new PhysicsDebug("physicsDebug"));
    this->AddToContext(physicsDebug);

    //Add debug class to world
    std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
    std::unique_ptr<btDiscreteDynamicsWorld> world = std::move(dynamics->GetWorld());
    world->setDebugDrawer(physicsDebug.get());
    dynamics->SetWorld(std::move(world));
#endif
}

void GLScene::playGame(int)
{
    // Play bgm, if it is not already playing
    if(bgmObject->state() == Phonon::StoppedState)
    {
        emit playBgm(); 
    }    

    // Play round start sound
    emit playEffect(1);

    this->update = false;
    this->resume();
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
    
    for(int i=0; i<this->entities->size(); i++)
    {
        // Get Dynamic Models
        std::shared_ptr<PhysicsModel> pmodel = entities->at(i)->GetPhysicsModel();
        std::shared_ptr<GLModel> gmodel = entities->at(i)->GetGraphicsModel();
        glm::mat4 transform = pmodel->GetTransform();


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
        matrices.mvpMatrix = vp * transform * gmodel->Matrix();
        matrices.mvMatrix = transform * gmodel->Matrix();
        matrices.normalMatrix = glm::transpose(glm::inverse(transform * gmodel->Matrix()));
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
        glBufferSubData( GL_UNIFORM_BUFFER, baseSize + 8, ptSize, &emissive->lights.point[0]);
        glBufferSubData( GL_UNIFORM_BUFFER, baseSize + ptSize + 16, sptSize, &(emissive->lights.spot[0]));
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
    
#ifdef PHYSICS_DEBUG
    {
        Matrices matrices;
        shared_ptr<GLProgram> pprogram = this->Get<GLProgram>("passthrough_program");
        shared_ptr<GLUniform> vuniform = this->Get<GLUniform>("GMatrices");
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

        // Repair Bug in Bullet Lib
        for(int i=0; i<this->entities->size(); i++)
        {
            entities->at(i)->GetPhysicsModel()->GetRigidBody()->applyDamping(dt/1.0f);
        }

        // Get Discrete Dynamics World and update time step
        std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
        std::unique_ptr<btDiscreteDynamicsWorld> world = std::move(dynamics->GetWorld());
        world->stepSimulation(dt/1.0f);
        dynamics->SetWorld(std::move(world));

        // Update all of the poo
        GLViewport::updateGL();

        // Update all of the physics dependent keys
        updateKeys();
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
    
    shared_ptr<GLEmissive> emissive = this->Get<GLEmissive>("lights");
    // Act on the key press event
    switch(event->key())
    {   
        case(Qt::Key_1):
            keyHeld[4] = !keyHeld[4];
            break;
        case(Qt::Key_2):
            keyHeld[5] = !keyHeld[5];
            break;
        case(Qt::Key_3):
            keyHeld[6] = !keyHeld[6];
            break;
        case(Qt::Key_4):
            keyHeld[7] = !keyHeld[7];
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
            emit mainMenu(0);
            break;
         case(Qt::Key_W):
            emissive->lights.basic.base.ambientIntensity +=0.05f;
            break;
        case(Qt::Key_S):
            emissive->lights.basic.base.ambientIntensity -=0.05f;
            break;
        case(Qt::Key_A):
            emissive->lights.basic.base.diffuseIntensity +=0.05f;
            break;
        case(Qt::Key_D):
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

void GLScene::playBgmWorker()
{
    // Play bgm
    bgmObject->setCurrentSource(*bgmSource);
    bgmObject->play();
}

void GLScene::playEffectWorker(int effectNum)
{
    // Check to see if there is an available sound output
    for(int i = 0; i < 5; i ++)
    {
        // Check if this mediaObject is able to play our effect
        if(mediaObjects[i]->state() == Phonon::StoppedState)
        {
            mediaObjects[i]->setCurrentSource(sources[effectNum]);
            mediaObjects[i]->play();
            break;
        }
    }
}

void GLScene::finished()
{
    bgmObject->play();
}

void GLScene::mousePressEvent(QMouseEvent *event)
{
   event->accept();
}

void GLScene::contextMenuEvent(QContextMenuEvent *event)
{
    GLViewport::contextMenuEvent(event);
}

GLScene::~GLScene()
{
    std::shared_ptr<DynamicsWorld> dynamics = this->Get<DynamicsWorld>("dynamics");
    std::unique_ptr<btDiscreteDynamicsWorld> world = std::move(dynamics->GetWorld());
    world.reset();
    dynamics->SetWorld(std::move(world));
}


