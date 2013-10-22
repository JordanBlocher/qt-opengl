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
        std::cout<<"Usage: " << argv[0] <<" <modelpath>\n"; //(options)\n\t\tOptions: <materialpath>\n";
        return 0;
    }

    QApplication app(argc, argv);
//    app.setPalette(QColor::fromRgbF(0.0, 0.0, 0.2));
//    QPalette palette = app.palette();
//    palette.setColor(QPalette::ButtonText, Qt::white);

    GLScene *glView = new GLScene(NULL, argc, argv);
    MainWindow *window = new MainWindow(NULL, glView);

    window->show();

    return app.exec();
}
	
