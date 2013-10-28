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
#include <QPalette>
#include <QAction>


#define WAIT_MS 300

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent)
{
    this->glView = static_cast<GLViewport*>( parent );
    this->setTransparent(true);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    
    this->layout = new QVBoxLayout;
    this->layout->setContentsMargins(140, 40, 0, 0);

    buttons[0] = new QPushButton(tr("Single Player"));
    buttons[0]->setMaximumWidth(120);
    layout->addWidget(buttons[0]);
    buttons[1] = new QPushButton(tr("Multi Player"));
    buttons[1]->setMaximumWidth(120);
    layout->addWidget(buttons[1]);
    buttons[2] = new QPushButton(tr("Quit"));
    buttons[2]->setMaximumWidth(120);
    layout->addWidget(buttons[2]);

    this->setLayout(layout);

    this->createActions();
}

MenuWidget::~MenuWidget()
{
}

void MenuWidget::show()
{
    QWidget::show();
}

void MenuWidget::createActions()
{
    this->exit = new QAction(tr("&Quit"), this);
    this->exit->setShortcuts(QKeySequence::Quit);
    connect(this->buttons[2], SIGNAL(clicked()), glView, SLOT(quit()));

    this->multi = new QAction(tr("&Multiplayer"), this);
    connect(this->buttons[1], SIGNAL(clicked()), this, SLOT(multiplayer()));
}

void MenuWidget::multiplayer()
{
    emit playGame(2);
}

void MenuWidget::singleplayer()
{
    emit playGame(1);
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
    painter.drawText(100, 40, tr("Play Airhockey"));
}

void MenuWidget::updatePaint()
{
    this->update();
    this->hide();
    this->show();
}

void MenuWidget::toggle()
{
    if( this->isVisible())
        this->hide();
    else
        this->show();
}

