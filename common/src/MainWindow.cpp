#include "MainWindow.hpp"
#include "GLViewport.hpp"
#include "OverlayWidget.hpp"
#include "DockWidget.hpp"
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


MainWindow::MainWindow(QWidget *parent, GLViewport *view, GLViewport *p1View, GLViewport *p2View) :
    QMainWindow(parent)
{

    this->layout = new QGridLayout;

    // Set up main window
    this->glView = view; //= ((view == NULL) ? new GLViewport(this) : view);
    setCentralWidget(glView);
    glView->setFocusPolicy(Qt::StrongFocus);
    glView->setFocus();
    setWindowTitle(tr("Air Hockey"));
    this->ok = true;
    this->layout->addWidget(glView);

    // Create Transparent Overlay
    overlay = new OverlayWidget(centralWidget());
    overlay->setBackgroundWidget(this);
    overlay->resize(this->width(), overlay->size().height() + 20);
    this->layout->addWidget(overlay);

    // Create Main Menu
    this->mainMenu = new MenuWidget(centralWidget());
    this->mainMenu->resize(this->width() / 2.0, this->height() - 200);
    this->layout->addWidget(mainMenu);

    this->setLayout(layout);

    // Menu stuff
    this->createActions();
    this->createMenus();
 
    // Connect user input for Player init
    connect( this, SIGNAL(setPlayer(Player, int)), overlay, SLOT(setPlayer(Player, int))); 

    this->getPlayer(this->p1, 1);
    this->getPlayer(this->p2, 2);
    this->createDockWindows();
    //this->createMainMenu();

    // Set up child windows
    connect( glView, SIGNAL(updateScore(int, int)), overlay, SLOT(updatePaint(int, int)));
    connect( dock, SIGNAL(updatePaddle(const char*, int)), glView, SLOT(updatePaddle(const char*, int)));
    connect(glView, SIGNAL(paint()), p1View, SLOT(paintCallback()));
    connect(glView, SIGNAL(key(QKeyEvent*)), p1View, SLOT(keyCallback(QKeyEvent*)));

    p1View->setFocusPolicy(Qt::NoFocus);
    //    p2View->setFocusPolicy(Qt::NoFocus);
 
    this->setChildViews(p1View, p2View);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setChildViews(GLViewport *child1View, GLViewport *child2View)
{
    std::shared_ptr<GLCamera> camera1 = child1View->Get<GLCamera>("camera");
    camera1->SetView(15.0f, -0.5f*M_PI, 0.4f*M_PI);
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

void MainWindow::createMainMenu()
{
    this->mainDock = new QDockWidget(NULL, this);
    this->mainDock->setAllowedAreas(Qt::TopDockWidgetArea);
    this->mainDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->qDock->setFixedHeight(this->height() - 200);
    this->dock = new DockWidget(this->height() / 200, this->width() / 2, qDock);
    this->qDock->setWidget(dock);

    this->addDockWidget(Qt::BottomDockWidgetArea, qDock);
  
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
    this->qDock = new QDockWidget(NULL, this);
    this->qDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    this->qDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->qDock->setFixedHeight(75);
    this->dock = new DockWidget(75, this->width(), qDock);
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
