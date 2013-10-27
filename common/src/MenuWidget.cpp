#include "MenuWidget.hpp"
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

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent, Qt::SubWindow)
{
    this->glView = static_cast<GLViewport*>( parent );
    this->setWindowFlags(windowFlags() | Qt::SubWindow);
    this->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    //this->setTransparent(true);
    
	buttons = new QGroupBox(tr("Options"));
    this->layout = new QVBoxLayout;

    buttons[0] = new QPushButton(tr("Single Player"));
    layout->addWidget(buttons[0]);
    buttons[1] = new QPushButton(tr("Multi Player"));
    layout->addWidget(buttons[1]);
    buttons[2] = new QPushButton(tr("Quit"));
    layout->addWidget(buttons[2]);

    buttons->setLayout(layout);
    this->setLayout(layout);

}

MenuWidget::~MenuWidget()
{
}

void MenuWidget::setBackgroundWidget(MainWindow *widget)
{
    window = widget;
    window->installEventFilter(this);
}

QDialog* MenuWidget::createDialog(const QString &windowTitle)
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

void MenuWidget::show()
{
    QWidget::show();
}

void MenuWidget::setTransparent(bool transparent)
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

void MenuWidget::setOpacity(const float &opacity)
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

void MenuWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.eraseRect(event->rect());
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 24));
    painter.drawText(this->width() / 2.0, 60, tr("Play Airhockey"));
}

// Filter events from user
// Returns false if the window is not grabbed
bool MenuWidget::eventFilter(QObject *obj, QEvent *event)
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

void MenuWidget::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::ActivationChange)
    {
        if(!isActiveWindow() & !window->isActiveWindow() )
            this->hide();
        else
            this->show();	

    }		
}
