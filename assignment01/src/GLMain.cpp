#include <GL/glew.h>
#include <QApplication>
#include <QWidget>
#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

#include <GLPrint.hpp>
#include "GLMain.hpp"

using namespace std;

GLMain::GLMain(QWidget *parent) : QGLApp(parent), angle(0.0){}

void GLMain::initializeGL()
{
    
    QGLApp::initializeGL();

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
    GLShader vertex = GLShader(GL_VERTEX_SHADER);
    GLShader fragment = GLShader(GL_FRAGMENT_SHADER);

    //Program
    unique_ptr<GLProgram> program(new GLProgram());

    //Add Shaders
    program->Add(vertex);
    program->Add(fragment);

    //Bind attribute index
    program->Bind("v_position", this->V_POSITION);
    program->Bind("v_color", this->V_COLOR);

    //--Init the view and projection matrices
    view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( this->FOV, float(this->size.width())/float(this->size.height()), this->sensor_distance, this->focal_distance);

    //enable depth testing (cool!)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //Add Program
    if( !this->Add(std::move(program)) )
    {
        cerr << "[F] Program failed to load."<<endl;
        qApp->quit();
        return;
    }

    //Bind uniforms
    this->program.front()->Bind("mvpMatrix", this->U_POSITION, 1, GL_UNIFORM_BUFFER, this->GLUniform);
}

void GLMain::paintGL()
{
    //clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Choose the shader program
    GLProgram &program = *(this->program.front());
    glUseProgram(program.getId());

    //premultiply the matrix for this example
    mvp = projection * view * model;

    GLint num;
    glGetProgramiv(program.getId(), GL_ACTIVE_UNIFORMS, &num);
    cout<<"num "<<num<<endl;
    GLuint m_pos = glGetUniformLocation(program.getId(), "mvpMatrix");
    cout<< "buf "<<m_pos<<endl;
    cout<<"GLbuf "<<this->U_POSITION<<endl;
    
    //upload the matrix to the shader
    glUniformMatrix4fv(this->U_POSITION, 1, GL_FALSE, glm::value_ptr(mvp));
    //glBindBuffer(GL_UNIFORM_BUFFER, GLUniform.buffer);
    //glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mvp));
    //glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
    model = glm::translate( glm::mat4(1.0f), glm::vec3(2.0 * sin(angle), 0.0, 2.0 * cos(angle))) * glm::rotate( glm::mat4(1.0f), float(180.0/M_PI) * angle, glm::vec3(0.0, 1.0, 0.0));
    
    QGLApp::idleGL();
}

void GLMain::resizeGL(int width, int height)
{
    QGLApp::resizeGL(width, height);
    projection = glm::perspective(this->FOV, float(width)/float(height), this->sensor_distance, this->focal_distance);
}

float GLMain::getDT()
{
    float ret;
    time = chrono::high_resolution_clock::now();
    ret = chrono::duration_cast< std::chrono::duration<float> >(time-start_time).count();
    start_time = chrono::high_resolution_clock::now();
    return ret;
}
