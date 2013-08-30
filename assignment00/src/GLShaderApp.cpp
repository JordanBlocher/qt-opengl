#include <GL/glew.h>
#include <iostream>

#include <QApplication>
#include <QKeyEvent>

#include "GLShaderApp.hpp"

using namespace std;

GLShaderApp::GLShaderApp(QWidget *parent) : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent), m_windowSize(640, 480)
{
   setMouseTracking(true);
}

GLShaderApp::~GLShaderApp()
{
   glDeleteProgram(m_program);
   glDeleteBuffers(1, &m_vboGeometry);
}

QSize GLShaderApp::minimumSizeHint() const
{
   return QSize(40,40);
}

QSize GLShaderApp::sizeHint() const
{
   return m_windowSize;
}

void GLShaderApp::initializeGL()
{
    // Initialize basic geometry and shaders for this example
    glewInit();

    //this defines a triangle, feel free to change it
    // each row is first the position of the vertex then the color
    Vertex geometry[] = { {{0.0, 0.9, 0.0}, {1.0, 0.0, 0.0}},
                          {{-0.9, -0.9, 0.0}, {0.0, 1.0, 0.0}},
                          {{0.9, -0.9, 0.0}, {0.0, 0.0, 1.0}}};

    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &m_vboGeometry);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboGeometry);
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
    m_program = glCreateProgram();
    glAttachShader(m_program, vertex_shader);
    glAttachShader(m_program, fragment_shader);
    glLinkProgram(m_program);
    //check if everything linked ok
    glGetProgramiv(m_program, GL_LINK_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        qApp->quit();
    }

    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
    m_locPosition = glGetAttribLocation(m_program,
                    const_cast<const char*>("v_position"));
    if(m_locPosition == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        qApp->quit();
    }

    m_locColor = glGetAttribLocation(m_program,
                    const_cast<const char*>("v_color"));
    if(m_locColor == -1)
    {
        std::cerr << "[F] V_COLOR NOT FOUND" << std::endl;
        qApp->quit();
    }
}

void GLShaderApp::paintGL()
{
    //clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //enable the shader program
    glUseProgram(m_program);

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(m_locPosition);
    glEnableVertexAttribArray(m_locColor);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboGeometry);
    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( m_locPosition,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    glVertexAttribPointer( m_locColor,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));

    glDrawArrays(GL_TRIANGLES, 0, 3);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(m_locPosition);
    glDisableVertexAttribArray(m_locColor);

    this->swapBuffers();
}

void GLShaderApp::resizeGL(int width __attribute__((unused)), int height __attribute__((unused)))
{
   // Not used in this example
}

void GLShaderApp::keyPressEvent(QKeyEvent *event)
{
   // Handle keyboard input
   if ( event->key() == Qt::Key_Escape )
   {
      qApp->quit();
   }
}

