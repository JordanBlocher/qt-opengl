#include "MainWindow.hpp"
#include "GLViewport.hpp"
#include "OverlayWidget.hpp"

#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QResizeEvent>
#include <QPalette>
#include <QColor>

MainWindow::MainWindow(QWidget *parent, GLViewport *view)
{
    this->glView = (view == NULL) ? new GLViewport(this) : view;
    setCentralWidget(glView);
    overlay = new OverlayWidget(centralWidget());
    overlay->setBackgroundWidget(this);
    overlay->resize(150, overlay->size().height());

    this->createActions();
    this->createMenus();
    connect( glView, SIGNAL(changeDirection(int)), overlay, SLOT( updatePaint(int) ) ); 
}

MainWindow::~MainWindow()
{
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

void MainWindow::resizeEvent(QResizeEvent *event)
{
    //glView->resizeApp(event->size().width(), event->size().height());
    //event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //glView->keyPressed(event);
    //event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
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