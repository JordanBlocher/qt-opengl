#ifndef MENU_H
#define MENU_H

#include <MenuWidget.hpp>

#include <QWidget>
#include <QString>

class MainWindow;
class QObject;
class QEvent;
class QLabel;
class QVBoxLayout;
class GLViewport;
class QPushButton;
class QStackedLayout;

class Menu : public MenuWidget
{
	Q_OBJECT

 public: 
	Menu(QWidget *parent = 0);
	~Menu();

 signals:
    void setModel(const char*);

 public slots:
    void updatePaint();
    void toggle(int);
    void openFile();

 protected: 
    void setConnections();
	void show();
    void paintEvent(QPaintEvent*);

    QString fileName;

};

#endif 

