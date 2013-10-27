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

class MenuWidget : public QWidget
{
	Q_OBJECT

 public: 
	MenuWidget(QWidget *parent = 0);
	~MenuWidget();

	void setBackgroundWidget(MainWindow *backGroundWidget);	

 public slots:

 protected: 
	virtual void changeEvent(QEvent*);
	virtual void show();
	bool eventFilter(QObject*, QEvent*);	
    void paintEvent(QPaintEvent*);
	QDialog* createDialog(const QString&);
	void setTransparent(bool transparent);
	void setOpacity(const float &opacity=0.8);

 private: 
	MainWindow *window;
    QVBoxLayout *layout;
    GLViewport *glView;
};

#endif 

