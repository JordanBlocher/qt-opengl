#include "MainWindow.hpp"
#include "GLViewport.hpp"
#include "OverlayWidget.hpp"
#include "GLCamera.hpp"
#include "MenuWidget.hpp"

#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QResizeEvent>
#include <QPalette>
#include <QColor>
#include <QWheelEvent>


MainWindow::MainWindow(QWidget *parent, GLViewport *view, MenuWidget *menu, const char* name) :
    QMainWindow(parent)
{

    this->name = name;
    // Set up main window
    this->glView = view; //= ((view == NULL) ? new GLViewport(this) : view);
    this->startState = view;
    setCentralWidget(glView);
    glView->setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(this->name);
    this->ok = true;

    if(this->name.toStdString() == std::string("Air Hockey"))
            this->scoreBoard();

    // Create Main Menu
    this->mainMenu = menu;
    if(this->name.toStdString() == std::string("Air Hockey"))
        this->mainMenu->resize(50, this->height() - 200);
    else if(std::string(name) == std::string("Lighting") || std::string(name) == std::string("Labyrinth"))
        this->mainMenu->resize(30, this->height() - 600);
    this->mainMenu->setFocusPolicy(Qt::StrongFocus);
    this->mainMenu->setFocus();

    if(std::string(name) == std::string("Air Hockey"))
        this->setConnections(1);
    else if(std::string(name) == std::string("Lighting"))
        this->setConnections(2);
    else if(std::string(name) == std::string("Labyrinth"))
        this->setConnections(3);

    // Menu stuff
    this->createActions();
    this->createMenus();
 
}

MainWindow::~MainWindow()
{
}

void MainWindow::setConnections(int menu)
{
    if(menu == 1 || menu == 3)
    {
        // Connections to main menu
        connect(mainMenu, SIGNAL(playGame(int)), this, SLOT(getPlayer(int)));
        connect(mainMenu, SIGNAL(resume()), glView, SLOT(resume()));
        if(menu == 1)
            connect(mainMenu, SIGNAL(changePaddle(int)), glView, SLOT(changePaddle(int)));

        connect(glView, SIGNAL(mainMenu(int)), mainMenu, SLOT(toggle(int)));
        connect(glView, SIGNAL(endGame()), this, SLOT(endGame()));

        // Connections to scoreboard
        connect(this, SIGNAL(playGame(int)), glView, SLOT(playGame(int)));
        connect(this, SIGNAL(pause()), glView, SLOT(pause()));
    }
    else if (menu == 2)
    {
        connect(glView, SIGNAL(mainMenu(int)), mainMenu, SLOT(toggle(int)));
        connect(mainMenu, SIGNAL(setModel(const char*)), glView, SLOT(setModel(const char*)));
    }

}

void MainWindow::scoreBoard()
{
    overlay = new OverlayWidget(glView);
    overlay->setBackgroundWidget(this);
    overlay->resize(this->width(), overlay->size().height() + 20);
    
    // Connections to display / user input
    glView->p1.name = "Player 1";
    glView->p1.score = 0;
    emit setPlayer(glView->p1, 1);
    glView->p2.name = "Player 2";
    glView->p2.score = 0;
    emit setPlayer(glView->p2, 2);
 
    connect( glView, SIGNAL(updateScore(int, int)), this, SLOT(updateScore(int, int)));
}

void MainWindow::createActions()
{
    this->exit = new QAction(tr("&Quit"), this);
    this->exit->setShortcuts(QKeySequence::Quit);
    connect(exit, SIGNAL(triggered()), this, SLOT(close()));

    this->about = new QAction(tr("&Help"), this);
    connect(about, SIGNAL(triggered()), this, SLOT(aboutCallback()));
    
    this->aboutQt = new QAction(tr("&About"), this);
    connect(aboutQt, SIGNAL(triggered()), this, SLOT(aboutQtCallback()));
}


void MainWindow::createMenus()
{
    this->infoLabel = new QLabel(tr("")); 

    this->menu = menuBar()->addMenu(tr("&Menu"));    
    this->menu->addAction(exit);

    this->menuBar()->addSeparator();

    this->help = menuBar()->addMenu(tr("&Help"));
    this->help->addAction(about);
    this->help->addAction(aboutQt);

}

QMenu* MainWindow::createPopupMenu()
{
    QMenu *menu = menuBar()->addMenu(tr(""));    
    menu->addAction(exit);
    menu->addAction(about);

    return menu;
}

void MainWindow::getPlayer(int i)
{
    if(i == 0)
    {
        this->mainMenu->hide();
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
        emit setPlayer(glView->p1, 1);
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
    emit setPlayer(glView->p2, 2);

    this->mainMenu->hide();
    glView->setFocus();

    emit playGame(i);

}

void MainWindow::updateScore(int score, int player)
{
    if(player == 1)
        glView->p1.score = score;
    else 
        glView->p2.score = score;
    overlay->updatePaint();
}

void MainWindow::endGame()
{
    glView->pause();
    if(glView->p1.score > glView->p2.score)
        glView->p1.winner = true;
    if(glView->p1.score < glView->p2.score)
        glView->p2.winner = true;
    mainMenu->toggle(2);
    mainMenu->updatePaint();
}

void MainWindow::resizeEvent(QResizeEvent* )
{
    if(this->name.toStdString() == std::string("Air Hockey"))
        overlay->resize(this->width(), overlay->size().height());

    this->mainMenu->resize(250, 250);

    this->mainMenu->move((glView->geometry().width()/2) -125, (glView->geometry().height()/2) -125 - 0);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::ActivationChange)
    {
    }
}

void MainWindow::keyPressEvent(QKeyEvent* )
{
}

void MainWindow::keyReleaseEvent(QKeyEvent* )
{
}

void MainWindow::mousePressEvent(QMouseEvent*)
{
}

void MainWindow::mouseMoveEvent(QMouseEvent*)
{
}

void MainWindow::wheelEvent(QWheelEvent*)
{
}

void MainWindow::aboutCallback()
{
    this->infoLabel->setText(tr("<b>Help|About</b>")); 
    QMessageBox::about(this, tr("Help"),
    tr("Right-click on the screen to start/stop the animation.\n"
    "Use the arrow keys <-> to rotate left and right, or \n"
    "use the space bar to toggle rotation."));
}

void MainWindow::aboutQtCallback()
{
    this->infoLabel->setText(tr("<b>Help|About Qt</b>"));
    QMessageBox::about(this, tr("About Qt"),
    tr("MainWindow \nQt Version 4.8.3"));

}

bool MainWindow::Ok()
{
    return this->ok;
}
