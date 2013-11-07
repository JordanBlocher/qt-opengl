#include <QApplication>
#include <QPalette>
#include <QColor>

#include <MainWindow.hpp>

#include "GameMenu.hpp"
#include "GLScene.hpp"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    app.setApplicationName("AirHockey");
    QPalette palette = app.palette();
    palette.setColor(QPalette::Window, QColor::fromRgbF(0.0, 0.0, 0.2));
    palette.setColor(QPalette::WindowText, Qt::white);
    app.setPalette(palette);

    GLScene *glView = new GLScene(1200, 800, NULL, argc, argv);
    GameMenu *menu = new GameMenu(glView);

    MainWindow *window = new MainWindow(NULL, glView, menu, "Air Hockey");
    if (!window->Ok())
        return 0;

    window->show();

    return app.exec();
}
	
