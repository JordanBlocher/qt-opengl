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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

#include <GLPrint.hpp>
#include <GLNode.hpp>
#include <GLShader.hpp>
#include <GLProgram.hpp>
#include <GLBufferObject.hpp>
#include <GLTransform.hpp>

#include "GLScene.hpp"

#define FOV 45.0f
#define SENSOR_DISTANCE 0.01f
#define FOCAL_DISTANCE 100.0f

using namespace std;

GLScene::GLScene(QWidget *parent) : angle(0.0), direction(1.0f), background(QColor::fromRgbF(0.0, 0.0, 0.2)), font(Qt::white), update(true)
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

cou<<"error?"<<endl;
    //Geometry (le sigh) 
    Vertex geometry[] = { {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},

                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}}
                        };

    // Create a Vertex Buffer object to store this vertex info on the GPU
    //shared_ptr<GLBufferObject> vbo(new GLBufferObject("Cube", sizeof(geometry), 1, 0, GL_ARRAY_BUFFER, GL_STATIC_DRAW));
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);


    //Shaders
    shared_ptr<GLShader> vertex(new GLShader(GL_VERTEX_SHADER, "vshader"));
    shared_ptr<GLShader> fragment(new GLShader(GL_FRAGMENT_SHADER, "fshader"));

    //Program
    shared_ptr<GLProgram> program(new GLProgram("qgl_program"));
    
    //Add Shaders
    program->AddShader(vertex);
    program->AddShader(fragment);

    //Bind attribute index
    program->SetAttributeIndex("v_position", this->V_POSITION);
    program->SetAttributeIndex("v_color", this->V_COLOR);

    // View matrix (eye pos, focus point, up)
    shared_ptr<GLTransform> view(new GLTransform("view", glm::lookAt(
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
    
    //Create UBO
    shared_ptr<GLBufferObject> ubo(new GLBufferObject("GMatrices", sizeof(glm::mat4), this->NUM_UNIFORMS, this->U_POSITION, GL_UNIFORM_BUFFER, GL_STREAM_DRAW));
    this->AddToContext(ubo);

    //Bind uniform index
    vector<string> uniforms = {"otherMatrix", "mvpMatrix"};i

    vector<shared_ptr<GLUniform>> uniforms;
    uniforms = program->SetUniformIndex(this->Get<GLBufferObject>("GMatrices"), uniforms, sizeof(glm::mat4), this->U_POSITION);
    this->AddToContext(uniforms[1]);
    

    //this->SetScene(perspective);

}

void GLScene::paintGL()
{
    
    // Clear the screen
    qreal r, g, b;
    this->background.getRgbF(&r, &g, &b);
    glClearColor(r, g, b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Choose the shader program
    shared_ptr<GLProgram> program = this->Get<GLProgram>("qgl_program");
    glUseProgram(program->getId());
    
    // Get view & projection matrices
    shared_ptr<GLTransform> projection = this->Get<GLTransform>("projection");
    shared_ptr<GLTransform> view = this->Get<GLTransform>("view");

    mvp = projection->Matrix() * view->Matrix();
    
    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(this->V_POSITION);
    glEnableVertexAttribArray(this->V_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( this->V_POSITION,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    glVertexAttribPointer( this->V_COLOR,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));

    glDrawArrays(GL_TRIANGLES, 0, 36);//mode, starting index, count

    //Select buffer
    shared_ptr<GLBufferObject> ubo = this->Get<GLBufferObject>("GMatrices");

    std::shared_ptr<GLUniform> mvpmat = this->Get("mvpMatrix");

    //Load buffer to MVP
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->getBuffer());
    glBufferSubData(GL_UNIFORM_BUFFER, mvpmat->Offset(), mvpmat->Size(), glm::value_ptr( mvp * cube ));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);//mode, starting index, count

    glBindBuffer(GL_UNIFORM_BUFFER, ubo->getBuffer());
    glBufferSubData(GL_UNIFORM_BUFFER, mvpmat->Offset(), mvpmat->Size(), glm::value_ptr( mvp * moon ));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(this->V_POSITION);
    glDisableVertexAttribArray(this->V_COLOR);

    glUseProgram(0);

}

void GLScene::idleGL()
{
    if( update )
    {
        float dt = getDT();

        angle += dt * M_PI/2;
        rotate += dt * M_PI/2 * direction;

        cube =    glm::translate( glm::mat4(1.0f), glm::vec3(4.0 * sin(angle/2.0), 0.0, 4.0 * cos(angle/2.0))) 
                * glm::rotate( glm::mat4(1.0f), float(180.0/M_PI) * -rotate, glm::vec3(0.0, 1.0, 0.0));

        moon =    glm::translate( glm::mat4(1.0f), glm::vec3(4.0 * sin(angle/2.0), 0.0, 4.0 * cos(angle/2.0)))
                * glm::translate( glm::mat4(1.0f), glm::vec3(2.0 * sin(1.5*angle), 0.0, 2.0 * cos(1.5*angle))) 
                * glm::rotate( glm::mat4(1.0f), float(180.0/M_PI) * rotate, glm::vec3(0.0, 1.0, 0.0))
                * glm::scale(mat4(1.0f), glm::vec3(0.3f));
        
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
    GLViewport::keyPressEvent(event);
    int sign = sgn<float>(this->direction);

    if( (event->key() == Qt::Key_Left) && (sign > 0))
    {
       this->direction*=-1.0;
    }
    if( (event->key() == Qt::Key_Right))
    {
       this->direction*=sign;
    }
    if( event->key() == Qt::Key_Space )
    {
       this->direction*=-1.0;
    }
    emit changeDirection(this->direction);
}

void GLScene::mousePressEvent(QMouseEvent *event)
{
    this->direction*=-1.0;
    emit changeDirection(this->direction);
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

