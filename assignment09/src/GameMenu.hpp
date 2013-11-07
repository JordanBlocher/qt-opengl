#ifndef GLGAMEMENU_H
#define GLGAMEMENU_H

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

class GameMenu : public MenuWidget
{
	Q_OBJECT

 public: 
	GameMenu(QWidget *parent = 0);
	~GameMenu();

 signals:
    void playGame(int);
    void changePaddle(int);

 public slots:
    void updatePaint();
    void toggle(int);

 protected: 
    void setConnections();
	void show();
    void paintEvent(QPaintEvent*);

 protected slots:
    void reset();
    void unpause();
    void multiplayer();
    void singleplayer();
    void getPaddle();

 private: 
};

#endif 

