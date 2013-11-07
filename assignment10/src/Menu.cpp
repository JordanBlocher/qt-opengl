#include "Menu.hpp"
#include "GLScene.hpp"

#include <GLViewport.hpp>

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
#include <QFileDialog>

#define WAIT_MS 300

Menu::Menu(QWidget *parent) : MenuWidget(parent)
{
    QPalette palette(this->palette());
    palette.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(palette);
    this->glView = static_cast<GLScene*>( parent );
    this->setWindowFlags(Qt::WindowStaysOnTopHint);

    this->layout = new QStackedLayout;
    this->layout->setAlignment(Qt::AlignCenter);

    // Main menu widget
    QWidget* main = new QWidget;
    this->mainLayout = new QVBoxLayout(main);
    this->mainLayout->setAlignment(Qt::AlignCenter);
    this->mainLayout->setSpacing(20);
    this->mainLayout->setContentsMargins(0, 30, 0, 0);
    main->move(this->geometry().center());
    buttons[0] = new QPushButton(tr("Select Model"));
    buttons[0]->setMaximumWidth(120);
    mainLayout->addWidget(buttons[0]);
    buttons[1] = new QPushButton(tr("Quit"));
    buttons[1]->setMaximumWidth(120);
    mainLayout->addWidget(buttons[1]);

    // Set up stacked menu
    this->layout->addWidget(main);

    this->layout->setCurrentIndex(0);
    this->setLayout(layout);

    this->setConnections();
}

Menu::~Menu()
{
}

void Menu::show()
{
    QWidget::show();
}

void Menu::openFile()
{
    this->fileName = QFileDialog::getOpenFileName(this,
            tr("Select Model"),
            "./");
    emit setModel(this->fileName.toStdString().c_str());
    this->hide();
}

void Menu::setConnections()
{
    connect(this->buttons[0], SIGNAL(clicked()), this, SLOT(openFile()));
    connect(this->buttons[1], SIGNAL(clicked()), glView, SLOT(quit()));
}

void Menu::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.eraseRect(event->rect());
    painter.setBrush(QBrush(QColor::fromRgbF(0.0, 0.0, 0.1)));
    QRectF rectangle(5.0, 5.0, this->geometry().width()-10, this->geometry().height()-10);
    painter.drawRect(rectangle);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Verdana", 24));
    //if(this->layout->currentIndex() == 0)   
    //painter.drawText(this->geometry().width()/2 - 100, 40, tr("Lighting"));
}

void Menu::updatePaint()
{
    MenuWidget::updatePaint();
}

void Menu::toggle(int i)
{
    MenuWidget::toggle(i);
}

