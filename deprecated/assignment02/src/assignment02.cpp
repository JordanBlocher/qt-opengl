#include <QApplication>
#include <GLWindow.hpp>

#include "GLScene.hpp"


int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   GLScene *glView = new GLScene;
   GLWindow *window = new GLWindow(0, glView);

   window->show();

   return app.exec();
}

