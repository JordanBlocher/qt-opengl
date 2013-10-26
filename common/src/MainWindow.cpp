#include "MainWindow.hpp"
#include "GLViewport.hpp"
#include "OverlayWidget.hpp"
#include "DockWidget.hpp"

#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QResizeEvent>
#include <QPalette>
#include <QColor>


MainWindow::MainWindow(QWidget *parent, GLViewport *view) :
    QMainWindow(parent)
{
    this->glView = ((view == NULL) ? new GLViewport(this) : view);
    setCentralWidget(glView);

    overlay = new OverlayWidget(centralWidget());
    overlay->setBackgroundWidget(this);
    overlay->resize(this->width(), overlay->size().height() + 35);

    setWindowTitle(tr("Air Hockey"));
    this->ok = true;

    this->createActions();
    this->createMenus();
    
    if (!connect( this, SIGNAL(setPlayer(Player, int)), overlay, SLOT(setPlayer(Player, int))) ) std::cout << "Failed to connect" << std::endl; 

    this->getPlayer(this->p1, 1);
    this->getPlayer(this->p2, 2);
    this->createDockWindows();

    if (!connect( glView, SIGNAL(updateScore(int, int)), overlay, SLOT(updatePaint(int, int)))) std::cout << "Failed to connect" << std::endl; 
    if (!connect( dock, SIGNAL(updatePaddle(const char*, int)), glView, SLOT(updatePaddle(const char*, int)))) std::cout << "Failed to connect" << std::endl; 

 
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

// Dock window for player options
void MainWindow::createDockWindows()
{
    this->qDock = new QDockWidget(tr("Player Options"), this);
    this->qDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    this->qDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->qDock->setMaximumHeight(200);
    this->dock = new DockWidget(qDock);
    this->qDock->setWidget(dock);

    this->addDockWidget(Qt::BottomDockWidgetArea, qDock);
    
}

void MainWindow::getPlayer(Player &p, int i)
{
    bool ok;
    QInputDialog *dialog = new QInputDialog();
    QString name = dialog->getText(this, 
                    tr(std::string("Player" + std::to_string(i)).c_str()),
                    tr("Name:"), 
                    QLineEdit::Normal, QDir::home().dirName(),
                    &ok);
    if (ok && !name.isEmpty())
    {
        p.name = name.toStdString();
        p.score = 0;
        emit setPlayer(p, i);
    }
    else ok = false;;
}

void MainWindow::resizeEvent(QResizeEvent* )
{
    overlay->resize(this->width(), overlay->size().height());
    overlay->updatePaint(0, 1);
    overlay->updatePaint(0, 2);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::ActivationChange)
    {
        if(this->overlay->isVisible())
            qDock->show();
        else
            qDock->hide();	

    }
}

void MainWindow::keyPressEvent(QKeyEvent* )
{
}

void MainWindow::mousePressEvent(QMouseEvent*)
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
