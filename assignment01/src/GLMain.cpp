#include <GL/glew.h>
#include <QApplication>
#include <QWidget>
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
#include <GLContext.hpp>
#include "GLMain.hpp"

#define FOV 45.0f
#define SENSOR_DISTANCE 0.01f
#define FOCAL_DISTANCE 100.0f

using namespace std;

GLMain::GLMain(QWidget *parent) : QGLView(parent), angle(0.0){}

void GLMain::initializeGL()
{
 
    QGLView::initializeGL();

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

    //--Init the view and projection matrices
    view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( FOV, float(this->size.width())/float(this->size.height()), SENSOR_DISTANCE, FOCAL_DISTANCE);

    //enable depth testing (cool!)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //Add Program
    if( !this->AddProgram(program) )
    {
        cerr << "[F] Program failed to link."<<endl;
        qApp->quit();
        exit(0);
    }
    this->AddToContext(program);
    
    //Create UBO
    shared_ptr<GLBufferObject> ubo(new GLBufferObject("GMatrices", sizeof(glm::mat4), this->NUM_UNIFORMS, this->U_POSITION, GL_UNIFORM_BUFFER));
    this->AddToContext(ubo);
    //Bind uniform index
    vector<string> uniforms = {"mvpMatrix"};

    if( !program->SetUniformIndex(this->Get<GLBufferObject>("GMatrices"), uniforms, sizeof(glm::mat4), this->U_POSITION))
    {
        cerr << "[F] UBO failed to bind."<<endl;
        qApp->quit();
        exit(0);
    }

}

void GLMain::paintGL()
{
    //clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Choose the shader program
    shared_ptr<GLProgram> program = this->Get<GLProgram>("qgl_program");
    glUseProgram(program->getId());
    
    //premultiply the matrix for this example
    mvp = projection * view * model;
    
    //Select buffer
    shared_ptr<GLBufferObject> ubo = this->Get<GLBufferObject>("GMatrices");

    //Load buffer to MVP
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->getBuffer());
    Uniform mvpmat = ubo->getUniform("mvpMatrix");
    glBufferSubData(GL_UNIFORM_BUFFER, mvpmat.offset, mvpmat.size, glm::value_ptr(mvp));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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

    //clean up
    glDisableVertexAttribArray(this->V_POSITION);
    glDisableVertexAttribArray(this->V_COLOR);

    glUseProgram(0);
}

void GLMain::idleGL()
{
    float dt = getDT();

    angle += dt * M_PI/2;
    model = glm::translate( glm::mat4(1.0f), glm::vec3(4.0 * sin(angle), 0.0, 4.0 * cos(angle))) * glm::rotate( glm::mat4(1.0f), float(180.0/M_PI) * angle, glm::vec3(0.0, 1.0, 0.0));
    
    QGLView::idleGL();
}

void GLMain::resizeGL(int width, int height)
{
    QGLView::resizeGL(width, height);
    projection = glm::perspective(FOV, float(width)/float(height), SENSOR_DISTANCE, FOCAL_DISTANCE);
}

float GLMain::getDT()
{
    float ret;
    time = chrono::high_resolution_clock::now();
    ret = chrono::duration_cast< std::chrono::duration<float> >(time-this->start_time).count();
    this->start_time = chrono::high_resolution_clock::now();
    return ret;
}
