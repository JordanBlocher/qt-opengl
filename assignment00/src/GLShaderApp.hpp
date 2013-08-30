#ifndef GLSHADERAPP_H
#define GLSHADERAPP_H

#include <QGLWidget>

class GLShaderApp : public QGLWidget
{
   Q_OBJECT

public:
   GLShaderApp(QWidget *parent = 0);
   ~GLShaderApp();

   QSize minimumSizeHint() const;
   QSize sizeHint() const;
protected:
   struct Vertex
   {
      GLfloat position[3];
      GLfloat color[3];
   };

   // Qt events (overloaded)
   void initializeGL();
   void paintGL();
   void resizeGL(int width, int height);
   void keyPressEvent(QKeyEvent *event);

   // Member variables
   GLuint m_program;
   GLuint m_vboGeometry;

   GLint m_locPosition;
   GLint m_locColor;

   QSize m_windowSize;
};

#endif // GLSHADERAPP_H

