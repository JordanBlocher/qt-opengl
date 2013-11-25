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
    void update();

 public slots:
    void updatePaint();
    void toggle(int);
    void updateScore(int, int);
    void endGame();

 protected: 
    void setConnections();
	void show();
    void paintEvent(QPaintEvent*);

 protected slots:
    void reset();
    void unpause();
    void play();

 private: 
};

#endif 

