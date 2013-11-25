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
    void resume();

 public slots:
    virtual void updatePaint();
    virtual void toggle(int);
    virtual void updateScore(int, int);

 protected: 
    virtual void setConnections();
	virtual void show();
    virtual void paintEvent(QPaintEvent*);

 protected slots:
    virtual void reset();
    virtual void unpause();

 protected: 
	MainWindow *window;
    QStackedLayout *layout;
    QVBoxLayout *mainLayout;
    QVBoxLayout *optionsLayout;
    QVBoxLayout *gameOverLayout;
    GLViewport *glView;
    QPushButton *buttons[7];
};

#endif 

