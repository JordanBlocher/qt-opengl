#include <GL/glew.h>
#include <QApplication>
#include <QWidget>
#include <iostream>

#include "GLMain.hpp"

GLMain::GLMain(QWidget *parent) : QGLApp(parent){}

void GLMain::initializeGL()
{
    QGLApp::initializeGL();

    //Geometry done
    //this defines a triangle, feel free to change it
    // each row is first the position of the vertex then the color
    Vertex geometry[] = { {{0.0, 0.9, 0.0}, {1.0, 0.0, 0.0}},
                          {{-0.9, -0.9, 0.0}, {0.0, 1.0, 0.0}},
                          {{0.9, -0.9, 0.0}, {0.0, 0.0, 1.0}}};

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

    //Add Program
    this->Add(std::move(program));
}

void GLMain::paintGL()
{
    //clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Choose the shader program
    glUseProgram(this->program.front()->getId());

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

    glDrawArrays(GL_TRIANGLES, 0, 3);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(this->V_POSITION);
    glDisableVertexAttribArray(this->V_COLOR);

    glUseProgram(0);

    this->swapBuffers();
}


