#include "GameMenu.hpp"
#include "GLScene.hpp"

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

GameMenu::GameMenu(QWidget *parent) : MenuWidget(parent)
{
    QPalette palette(this->palette());
    palette.setColor(QPalette::Background, Qt::red);
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
    buttons[0] = new QPushButton(tr("Single Player"));
    buttons[0]->setMaximumWidth(120);
    mainLayout->addWidget(buttons[0]);
    buttons[1] = new QPushButton(tr("Multi Player"));
    buttons[1]->setMaximumWidth(120);
    mainLayout->addWidget(buttons[1]);
    buttons[2] = new QPushButton(tr("Quit"));
    buttons[2]->setMaximumWidth(120);
    mainLayout->addWidget(buttons[2]);

    // Options widget
    QWidget* options = new QWidget;
    this->optionsLayout = new QVBoxLayout(options);
    this->optionsLayout->setAlignment(Qt::AlignCenter);
    this->optionsLayout->setSpacing(20);
    this->optionsLayout->setContentsMargins(0, 30, 0, 0);
    buttons[3] = new QPushButton(tr("Change Paddle"));
    buttons[3]->setMaximumWidth(120);
    optionsLayout->addWidget(buttons[3]);
    buttons[4] = new QPushButton(tr("Resume"));
    buttons[4]->setMaximumWidth(120);
    optionsLayout->addWidget(buttons[4]);
    buttons[5] = new QPushButton(tr("Main Menu"));
    buttons[5]->setMaximumWidth(120);
    optionsLayout->addWidget(buttons[5]);

    // Game Over widget
    QWidget* gameOver = new QWidget;
    this->gameOverLayout = new QVBoxLayout(gameOver);
    this->gameOverLayout->setAlignment(Qt::AlignCenter);
    this->gameOverLayout->setSpacing(20);
    this->gameOverLayout->setContentsMargins(0, 30, 0, 0);
    buttons[6] = new QPushButton(tr("Main Menu"));
    buttons[6]->setMaximumWidth(120);
    gameOverLayout->addWidget(buttons[6]);

    // Set up stacked menu
    this->layout->addWidget(main);
    this->layout->addWidget(options);
    this->layout->addWidget(gameOver);

    this->layout->setCurrentIndex(0);
    this->setLayout(layout);

    this->setConnections();
}

GameMenu::~GameMenu()
{
}

void GameMenu::show()
{
    QWidget::show();

}

void GameMenu::setConnections()
{
    connect(this->buttons[0], SIGNAL(clicked()), this, SLOT(singleplayer()));
    connect(this->buttons[1], SIGNAL(clicked()), this, SLOT(multiplayer()));
    connect(this->buttons[2], SIGNAL(clicked()), glView, SLOT(quit()));
    connect(this->buttons[3], SIGNAL(clicked()), this, SLOT(getPaddle()));
    connect(this->buttons[4], SIGNAL(clicked()), this, SLOT(unpause()));
    connect(this->buttons[5], SIGNAL(clicked()), this, SLOT(reset()));
    connect(this->buttons[6], SIGNAL(clicked()), this, SLOT(reset()));
}

void GameMenu::getPaddle()
{
      QStringList items;
      items << tr("Circle") << tr("Square") << tr("Triangle");
      bool ok;
      QString item = QInputDialog::getItem(this, tr("Change Paddle"), tr("Paddle Types:"), items, 0, false, &ok);

      if(ok)
          emit changePaddle(items.indexOf(item)); 
}


void GameMenu::multiplayer()
{
    emit playGame(2);
}

void GameMenu::singleplayer()
{
    emit playGame(1);
}

void GameMenu::reset()
{
    MenuWidget::reset();
}

void GameMenu::unpause()
{
    MenuWidget::unpause();
}

void GameMenu::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.eraseRect(event->rect());
    painter.setBrush(QBrush(Qt::white));
     QRectF rectangle(5.0, 5.0, this->geometry().width()-10, this->geometry().height()-10);
    painter.drawRect(rectangle);
    painter.setPen(Qt::darkGray);
    painter.setFont(QFont("Trebuchet", 24));
    if(this->layout->currentIndex() == 0)   
        painter.drawText(this->geometry().width()/2 - 90, 40, tr("Air Hockey!"));
    if(this->layout->currentIndex() == 1)   
        painter.drawText(this->geometry().width()/2 - 110, 40, tr("Options Menu"));
    if(this->layout->currentIndex() == 2)   
    {
        painter.setFont(QFont("Arial", 28, QFont::Bold, QFont::SmallCaps));
        painter.drawText(this->geometry().width()/2 - 100, 40, tr("Game Over"));
        painter.setPen(Qt::darkGray);
        painter.setFont(QFont("Arial", 18, QFont::Bold, QFont::SmallCaps));
        if(glView->p1.winner)
            painter.drawText(this->geometry().width()/2 - 100, 80, std::string(glView->p1.name + "  Won!").c_str());
        else if(glView->p2.winner)
            painter.drawText(this->geometry().width()/2 - 100, 80, std::string(glView->p2.name + "  Won!").c_str());
    }
}

void GameMenu::updatePaint()
{
    MenuWidget::updatePaint();
}

void GameMenu::toggle(int i)
{
    MenuWidget::toggle(i);
}

