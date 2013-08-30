#include <QApplication>

#include "GLShaderApp.hpp"

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   GLShaderApp window;
   window.show();

   return app.exec();
}

