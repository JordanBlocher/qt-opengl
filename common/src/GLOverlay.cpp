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

#define WAIT_MS 300

#include "GLOverlay.hpp"
#include "GLWindow.hpp"
#include "GLViewport.hpp"

GLOverlay::GLOverlay(QWidget *parent) : QWidget(parent, Qt::SubWindow)
{
    this->glView = static_cast<GLViewport*>( parent );
    this->setWindowFlags(windowFlags() | Qt::SubWindow);
    this->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    this->setTransparent(true);

    QToolButton *rarrow = new QToolButton();
    rarrow->setArrowType(Qt::RightArrow);
    QToolButton *larrow = new QToolButton();
    larrow->setArrowType(Qt::LeftArrow);
    //this->info->setPalette(QColor(0.0, 0.0, 0.2));
    //layout = new QHBoxLayout;

    //this->setLayout(this->layout);
    this->msg = "QT OpenGL";
}

GLOverlay::~GLOverlay()
{
}

void GLOverlay::setBackgroundWidget(GLWindow *widget)
{
    window = widget;
    window->installEventFilter(this);
}

QDialog* GLOverlay::createDialog(const QString &windowTitle)
{
    QDialog *dialog = new QDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    dialog->setWindowTitle(windowTitle);
    dialog->setLayout(new QVBoxLayout);
    dialog->setWindowTitle(windowTitle);

    dialog->setAttribute(Qt::WA_TranslucentBackground);
    dialog->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    dialog->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0);
    return dialog;
}

void GLOverlay::show()
{
    QWidget::show();
}

void GLOverlay::setTransparent(bool transparent)
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

void GLOverlay::setOpacity(const float &opacity)
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

void GLOverlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.eraseRect(event->rect());
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    //painter.drawText(event->rect(), Qt::AlignRight, this->msg);
    painter.drawText(0,20,this->msg);
}

// Filter events from user
// Returns false if the window is not grabbed
bool GLOverlay::eventFilter(QObject *obj, QEvent *event)
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

void GLOverlay::updatePaint(int direction)
{
    if( direction > 0 )
        this->msg = "CounterClockwise";
    else
        this->msg = "Clockwise";
    this->update();
    this->hide();
    this->show();
}

void GLOverlay::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::ActivationChange)
    {
        if(!isActiveWindow() & !window->isActiveWindow() )
            this->hide();
        else
            this->show();	

    }		
}
