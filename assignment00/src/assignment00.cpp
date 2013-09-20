#include <QApplication>

#include "GLScene.hpp"

int main(int argc, char *argv[])
{
   int ret = 0;

   QApplication app(argc, argv);

   GLViewport *window = new GLScene();
   window->show();

   ret = app.exec();

   delete window;

   return ret;
}

