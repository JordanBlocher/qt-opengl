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
    MainWindow(QWidget *parent = 0, GLViewport *view = NULL);
    ~MainWindow();

    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
    void mousePressEvent(QMouseEvent*);
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

 protected:
    virtual QMenu* createPopupMenu();

    // Initialize Game
    Player p1;
    Player p2;
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
    void aboutCallback();     
    void aboutQtCallback();
    void getPlayer(int);

 private:
    void createActions();
    void createMenus();
    void setChildViews(GLViewport*, GLViewport*);

    bool ok;


};


#endif 

