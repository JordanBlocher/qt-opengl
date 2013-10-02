#include <QApplication>

#include "GLScene.hpp"

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   GLScene window;
   window.show();

   return app.exec();
}

