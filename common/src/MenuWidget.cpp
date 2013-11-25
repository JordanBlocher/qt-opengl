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
#include <QStackedLayout>
#include <QInputDialog>
#include <QSpacerItem>

#define WAIT_MS 300

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent)
{
}

MenuWidget::~MenuWidget()
{
}

void MenuWidget::show()
{
    QWidget::show();

}

void MenuWidget::setConnections()
{
}

void MenuWidget::updateScore(int, int)
{
}

void MenuWidget::forceRepaint()
{
}

void MenuWidget::reset()
{
    this->updatePaint();
    this->layout->setCurrentIndex(0);
    this->updatePaint();
}

void MenuWidget::unpause()
{
    this->hide();
    this->glView->setFocus();
    emit resume();
}

void MenuWidget::paintEvent(QPaintEvent *)
{
}

void MenuWidget::updatePaint()
{
    this->update();
    this->hide();
    this->show();
}

void MenuWidget::toggle(int i)
{
    this->layout->setCurrentIndex(i);
    if( this->isVisible())
        this->hide();
    else
        this->show();
}

