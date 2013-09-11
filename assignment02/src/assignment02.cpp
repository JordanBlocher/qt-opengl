#include <QApplication>
#include <QGLApp.hpp>

#include "GLMain.hpp"


int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   GLMain *glView = new GLMain;
   QGLApp *window = new QGLApp(0, glView);

   window->show();

   return app.exec();
}

