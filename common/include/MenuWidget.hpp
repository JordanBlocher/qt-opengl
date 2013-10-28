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

class MenuWidget : public QWidget
{
	Q_OBJECT

 public: 
	MenuWidget(QWidget *parent = 0);
	~MenuWidget();

 signals:
    void playGame(int);

 public slots:
    void updatePaint();
    void toggle();

 protected: 
    void createActions();
	virtual void show();
    void paintEvent(QPaintEvent*);
	void setTransparent(bool transparent);
	void setOpacity(const float &opacity=0.8);

    QAction *single;
    QAction *multi;
    QAction *exit;

 protected slots:
    void multiplayer();
    void singleplayer();

 private: 
	MainWindow *window;
    QVBoxLayout *layout;
    GLViewport *glView;
    QPushButton *buttons[3];
};

#endif 

