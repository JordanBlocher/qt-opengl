#ifndef GLMENU_H
#define GLMENU_H

#include "GLStruct.hpp"

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

class MenuWidget : public QWidget
{
	Q_OBJECT

 public: 
	MenuWidget(QWidget *parent = 0);
	~MenuWidget();

 signals:
    void playGame(int);
    void changePaddle(int);
    void start();

 public slots:
    void updatePaint();
    void toggle(int);

 protected: 
    void setConnections();
	virtual void show();
    void paintEvent(QPaintEvent*);
	void setTransparent(bool transparent);
	void setOpacity(const float &opacity=0.8);

    QAction *single;
    QAction *multi;
    QAction *exit;

 protected slots:
    void reset();
    void resume();
    void multiplayer();
    void singleplayer();
    void getPaddle();

 private: 
	MainWindow *window;
    QStackedLayout *layout;
    QVBoxLayout *mainLayout;
    QVBoxLayout *optionsLayout;
    GLViewport *glView;
    QPushButton *buttons[6];
};

#endif 

