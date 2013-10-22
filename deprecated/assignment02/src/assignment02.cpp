#include <QApplication>
#include <MainWindow.hpp>

#include "GLScene.hpp"


int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   GLScene *glView = new GLScene;
   MainWindow *window = new MainWindow(0, glView);

   window->show();

   return app.exec();
}

