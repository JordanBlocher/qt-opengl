#include "OverlayWidget.hpp"
#include "MainWindow.hpp"
#include "GLViewport.hpp"

#include <QTime>
#include <QWindowStateChangeEvent>
#include <QLayout>
#include <QPushButton>
#include <QToolButton>
#include <QGraphicsOpacityEffect>
#include <QObject>
#include <QMainWindow>
#include <QPainter>
#include <QLabel>
#include <QDialog>
#include <QPalette>
#include <sstream>
#include <QDebug>

#define WAIT_MS 300

OverlayWidget::OverlayWidget(QWidget *parent) : QWidget(parent, Qt::SubWindow)
{
    this->glView = static_cast<GLViewport*>( parent );
    this->setWindowFlags(windowFlags() | Qt::SubWindow);
    this->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    this->setTransparent(true);

    QToolButton *rarrow = new QToolButton();
    rarrow->setArrowType(Qt::RightArrow);
    QToolButton *larrow = new QToolButton();
    larrow->setArrowType(Qt::LeftArrow);

}

OverlayWidget::~OverlayWidget()
{
}

void OverlayWidget::setBackgroundWidget(MainWindow *widget)
{
    window = widget;
    window->installEventFilter(this);
}

void OverlayWidget::show()
{
    QWidget::show();
}

void OverlayWidget::setTransparent(bool transparent)
{
    if (transparent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
        setOpacity();

        this->hide();
    }
    else
    {
        setAttribute(Qt::WA_TranslucentBackground,false);
        setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);

        this->show();
    }
}

void OverlayWidget::setOpacity(const float &opacity)
{
    // Disabled to get rid of that nasty effect. May (wont) fix later
    //QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    //this->setGraphicsEffect(opacityEffect);
    //opacityEffect->setOpacity(opacity);
}

void OverlayWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.eraseRect(event->rect());
    painter.setBrush(QBrush(Qt::darkGray));
    QRectF rectangle(2.0, 2.0, 120, this->geometry().height()-10);
    painter.drawRect(rectangle);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(5,20,glView->p1.name.c_str());
    painter.drawText(15,40,std::to_string(glView->p1.score).c_str());
    if(glView->p2.score >= 0)
    {
        QRectF rectangle2(this->geometry().width()-122, 2.0, 120, this->geometry().height()-10);
        painter.drawRect(rectangle2);
        painter.drawText(this->width() - 11*glView->p2.name.length(),20,glView->p2.name.c_str());
        painter.drawText(this->width() - 25,40,std::to_string(glView->p2.score).c_str());
    }

    // Time parabox
    QRectF rectangle3(this->geometry().width()/2, 2.0, 120, this->geometry().height()-10);
    painter.drawRect(rectangle3);
    std::stringstream ss;
    ss << "Time: "<< glView->gameTime;
    painter.drawText(this->geometry().width()/2, 30, ss.str().c_str());
}

// Filter events from user
// Returns false if the window is not grabbed
bool OverlayWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj==window)
    {
        if (event->type() == QEvent::Show)
            this->show();

        if (event->type() == QEvent::Close)
        {
            window = NULL;
            this->close();
        }

        if (event->type()==QEvent::WindowStateChange)
        {
            if (!window->isMinimized())
            {
                QWindowStateChangeEvent* state = static_cast<QWindowStateChangeEvent*>( event );
                if (state->oldState() & Qt::WindowMinimized) 
                {
                    QTime dieTime= QTime::currentTime().addMSecs(WAIT_MS);
                    while( QTime::currentTime() < dieTime );
                }

                this->show();
            }
            else this->hide();
        }

        if (event->type()==QEvent::ActivationChange)
        {
            if(!window->isActiveWindow() & !isActiveWindow() )
                this->hide();
            else this->show();
        }
    }

    return false;
}

void OverlayWidget::updatePaint()
{
    this->update();
    this->hide();
    this->show();
}

void OverlayWidget::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::ActivationChange)
    {
        if(!isActiveWindow() & !window->isActiveWindow() )
            this->hide();
        else
            this->show();	

    }		
}


