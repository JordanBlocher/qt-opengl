#ifndef GLOVERLAY_H
#define GLOVERLAY_H

#include "GLStruct.hpp"

#include <QWidget>
#include <QString>

class MainWindow;
class QObject;
class QEvent;
class QLabel;
class QHBoxLayout;
class GLViewport;

class OverlayWidget : public QWidget
{
	Q_OBJECT

 public: 
	OverlayWidget(QWidget *parent = 0);
	~OverlayWidget();

	void setBackgroundWidget(MainWindow *backGroundWidget);	

 public slots:
    void updatePaint();

 protected: 
	virtual void changeEvent(QEvent*);
	virtual void show();
	bool eventFilter(QObject*, QEvent*);	
    void paintEvent(QPaintEvent*);
	void setTransparent(bool transparent);
	void setOpacity(const float &opacity=0.8);

 private: 
	MainWindow *window;
    QHBoxLayout *layout;
    GLViewport *glView;
};

#endif 

