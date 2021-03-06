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
#include <QMessageBox>
#include <QDir>

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

void GameMenu::endGame()
{
    glView->pause();
    if(glView->p1.score > glView->p2.score)
        glView->p1.winner = true;
    if(glView->p1.score < glView->p2.score)
        glView->p2.winner = true;
    this->toggle(2);
    this->updatePaint();
}

void GameMenu::getPlayer(int i)
{
    if(i == 0)
    {
        this->hide();
        glView->setFocus();
        emit playGame(0);
        return;
    }
    bool ok;
    QInputDialog *dialog1 = new QInputDialog();
    QString name = dialog1->getText(this, 
                    tr(std::string("Player" + std::to_string(i)).c_str()),
                    tr("Name:"), 
                    QLineEdit::Normal, QDir::home().dirName(),
                    &ok);
    if (ok && !name.isEmpty())
    {
        glView->p1.name = name.toStdString();
        glView->p1.score = 0;
        glView->p1.winner = false;
    }
    else ok = false;

    if(i > 1)
    {
        QInputDialog *dialog2 = new QInputDialog();
        QString name = dialog2->getText(this, 
                    tr(std::string("Player" + std::to_string(i)).c_str()),
                    tr("Name:"), 
                    QLineEdit::Normal, QDir::home().dirName(),
                    &ok);
        if (ok && !name.isEmpty())
        {
            glView->p2.name = name.toStdString();
            glView->p2.score = 0;
        
        }
        else ok = false;
    }
    else
    {
        glView->p2.name = "Computer";
        glView->p2.score = 0;
        glView->p2.winner = false;
    }

    this->hide();
    glView->setFocus();

    emit playGame(i);

}

void GameMenu::updateScore(int score, int player)
{
    if(player == 1)
        glView->p1.score = score;
    else 
        glView->p2.score = score;
    emit update();
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
    this->getPlayer(2);
}

void GameMenu::singleplayer()
{
    this->getPlayer(1);
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

