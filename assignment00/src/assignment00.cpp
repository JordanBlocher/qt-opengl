#include <QApplication>

#include "GLMain.hpp"

int main(int argc, char *argv[])
{
   int ret = 0;

   QApplication app(argc, argv);

   QGLApp *window = new GLMain();
   window->show();

   ret = app.exec();

   delete window;

   return ret;
}

