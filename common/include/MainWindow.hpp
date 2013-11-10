#ifndef QGLAPP_H
#define QGLRAPP_H

#include "GLStruct.hpp"

#include <QMainWindow>

class QAction;
class QLabel;
class QMenu;
class GLViewport;
class OverlayWidget;
class QResizeEvent;
class QColor;
class DockWidget;
class MenuWidget;
class QGridLayout;


class MainWindow : public QMainWindow
{
    Q_OBJECT

 public:
    MainWindow(QWidget *parent = 0, GLViewport *view = NULL, MenuWidget *menu = NULL, const char* = "");
    ~MainWindow();

    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);

    void changeEvent(QEvent*);

    GLViewport *glView;
    GLViewport *startState;
    OverlayWidget *overlay;
    MenuWidget *mainMenu;
    QDockWidget *mainDock;

    bool Ok();

 signals:
   void setPlayer(Player, int);
   void playGame(int);
   void playSound();
   void pause();

 protected:
    virtual QMenu* createPopupMenu();

    // Initialize Game
    DockWidget *dock;
    QDockWidget *qDock;

    // Default Menu Options
    QMenu *menu;
    QMenu *help;
    QAction *exit;
    QAction *start;
    QAction *single;
    QAction *multi;
    QAction *about;
    QAction *aboutQt;
    QLabel *infoLabel;
    QPalette *palette;
    
    // Player options for dock widgets
    QAction *child1Select;
    QAction *child2Select;
   
 protected slots:
    void setConnections(int);
    void scoreBoard();
    void aboutCallback();     
    void aboutQtCallback();
    void getPlayer(int);
    void updateScore(int, int);
    void endGame();

 private:
    void createActions();
    void createMenus();

    bool ok;
    QString name;


};


#endif 

