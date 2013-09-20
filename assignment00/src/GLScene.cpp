#include <GL/glew.h>
#include <QApplication>
#include <QWidget>
#include <iostream>

#include <GLStruct.hpp>

#include "GLScene.hpp"

GLScene::GLScene(QWidget *parent) : GLViewport(parent){}

void GLScene::initializeGL()
{

    GLViewport::initializeGL();


    //this defines a triangle, feel free to change it
    // each row is first the position of the vertex then the color
    Vertex geometry[] = { {{0.0, 0.9, 0.0}, {1.0, 0.0, 0.0}},
                          {{-0.9, -0.9, 0.0}, {0.0, 1.0, 0.0}},
                          {{0.9, -0.9, 0.0}, {0.0, 0.0, 1.0}}};

    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);

    //--Geometry done

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    //Shader Sources
    // Put these into files and write a loader in the future
    const char *vs =
        "attribute vec3 v_position;"
        "attribute vec3 v_color;"
        "varying vec3 color;"
        "void main(void){"
        "   gl_Position = vec4(v_position, 1.0);"
        "   color = v_color;"
        "}";


    const char *fs =
        "varying vec3 color;"
        "void main(void){"
        "   gl_FragColor = vec4(color.xyz, 1.0);"
        "}";

    //compile the shaders
    GLint shader_status;

    // Vertex shader first
    glShaderSource(vertex_shader, 1, &vs, NULL);
    glCompileShader(vertex_shader);
    //check the compile status
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] FAILED TO COMPILE VERTEX SHADER!" << std::endl;
        qApp->quit();
    }

    // Now the Fragment shader
    glShaderSource(fragment_shader, 1, &fs, NULL);
    glCompileShader(fragment_shader);
    //check the compile status
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] FAILED TO COMPILE FRAGMENT SHADER!" << std::endl;
        qApp->quit();
    }

    //Now we link the 2 shader objects into a program
    //This program is what is run on the GPU
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    
    glBindAttribLocation(program, 1, "v_position");
    glBindAttribLocation(program, 2, "v_color");

    glLinkProgram(program);
    //check if everything linked ok
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        qApp->quit();
    }
    
    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
    position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(position == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        qApp->quit();
    }

    color = glGetAttribLocation(program,
                    const_cast<const char*>("v_color"));
    if(color == -1)
    {
        std::cerr << "[F] V_COLOR NOT FOUND" << std::endl;
        qApp->quit();
    }

}

void GLScene::paintGL()
{
    //clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //enable the shader program
    glUseProgram(program);

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    glVertexAttribPointer( color,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));

    glDrawArrays(GL_TRIANGLES, 0, 3);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(position);
    glDisableVertexAttribArray(color);

    this->swapBuffers();
}


