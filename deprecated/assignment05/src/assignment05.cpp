#include <QApplication>
#include <QPalette>
#include <QColor>

#include <MainWindow.hpp>
#include <OverlayWidget.hpp>

#include "GLScene.hpp"


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cout<<"Usage: assignment05 <modelpath>\n"; //(options)\n\t\tOptions: <materialpath>\n";
        return 0;
    }

    QApplication app(argc, argv);
    QPalette palette = app.palette();
    palette.setColor(QPalette::Window, QColor::fromRgbF(0.0, 0.0, 0.2));
    palette.setColor(QPalette::WindowText, Qt::white);
    app.setPalette(palette);

    GLScene *glView = new GLScene(1200, 800, NULL, argc, argv);

    MainWindow *window = new MainWindow(NULL, glView);


    if (!window->Ok())
        return 0;

    window->show();


    return app.exec();
}
	
