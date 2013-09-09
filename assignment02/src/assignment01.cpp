#include <QApplication>

#include "GLMain.hpp"

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   GLMain window;
   window.show();

   return app.exec();
}

