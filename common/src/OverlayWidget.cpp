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
    }
    else
    {
        setAttribute(Qt::WA_TranslucentBackground,false);
        setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
    }
}

void OverlayWidget::setOpacity(const float &opacity)
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

void OverlayWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.eraseRect(event->rect());
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(5,20,this->p1.name.c_str());
    painter.drawText(15,40,std::to_string(this->p1.score).c_str());
    painter.drawText(this->width() - 10*this->p2.name.length(),20,this->p2.name.c_str());
    painter.drawText(this->width() - 10*this->p2.name.length() + 15,40,std::to_string(this->p2.score).c_str());
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

void OverlayWidget::updatePaint(int score, int player)
{
    if(player == 1)
        p1.score = score;
    else if (player == 2)
        p2.score = score;
    this->update();
    this->hide();
    this->show();
}

void OverlayWidget::setPlayer(Player p, int player)
{
    if (player == 1)
    {
        p1 = p;
    }
    else if (player == 2)
    {
        p2 = p;
    }
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
