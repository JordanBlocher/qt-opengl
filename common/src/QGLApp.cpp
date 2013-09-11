#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <QMenu>
#include <QAction>

#include "QGLApp.hpp"
#include "QGLView.hpp"

QGLApp::QGLApp(QWidget *parent, QGLView *view)
{
    this->glView = (view == NULL) ? new QGLView(this) : view;
    setCentralWidget(glView);

    this->createActions();
    this->createMenus();
}

QGLApp::~QGLApp()
{
}

void QGLApp::createActions()
{
    this->exit = new QAction(tr("&Quit"), this);
    this->exit->setShortcuts(QKeySequence::Quit);
    this->exit->setStatusTip(tr("Quit the application"));
    connect(exit, SIGNAL(triggered()), this, SLOT(close()));

    this->about = new QAction(tr("&Help"), this);
    this->about->setStatusTip(tr("Running the application"));
    connect(about, SIGNAL(triggered()), this, SLOT(aboutCallback()));
    
    this->aboutQt = new QAction(tr("&About"), this);
    this->aboutQt->setStatusTip(tr("App Info"));
    connect(aboutQt, SIGNAL(triggered()), this, SLOT(aboutQtCallback()));
}


void QGLApp::createMenus()
{
    this->menu = menuBar()->addMenu(tr("&Menu"));    
    this->menu->addAction(exit);

    this->menuBar()->addSeparator();

    this->help = menuBar()->addMenu(tr("&Help"));
    this->help->addAction(about);
    this->help->addAction(aboutQt);
}


QMenu* QGLApp::createPopupMenu()
{
    QMenu *menu = menuBar()->addMenu(tr(""));    
    menu->addAction(exit);
    menu->addAction(about);

    return menu;
}

void QGLApp::aboutCallback()
{
    this->infoLabel = new QLabel(tr("<b>Help|About</b>")); 
    QMessageBox::about(this, tr("Help"),
    tr("Right-click on the screen to start/stop the animation.\n"
    "Use the arrow keys <-> to rotate left and right, or \n"
    "use the space bar to toggle rotation."));
}

void QGLApp::aboutQtCallback()
{
    this->infoLabel->setText(tr("<b>Help|About Qt</b>"));
    QMessageBox::about(this, tr("About Qt"),
    tr("QGLApp \nQt Version 4.8.3"));

}
