#include <QApplication>
#include <QPalette>
#include <QColor>

#include <GLWindow.hpp>
#include <GLOverlay.hpp>

#include "GLScene.hpp"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setPalette(QColor::fromRgbF(0.0, 0.0, 0.2));
    QPalette palette = app.palette();
    palette.setColor(QPalette::ButtonText, Qt::white);

    GLScene *glView = new GLScene(NULL);
    GLWindow *window = new GLWindow(NULL, glView);

    window->show();

    return app.exec();
}
	
