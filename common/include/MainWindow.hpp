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

class MainWindow : public QMainWindow
{
Q_OBJECT

 public:
    MainWindow(QWidget *parent = 0, GLViewport *view = NULL);
    ~MainWindow();

    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent*);
    void mousePressEvent(QMouseEvent*);
    void changeEvent(QEvent*);

    GLViewport *glView;
    OverlayWidget *overlay;

    bool Ok();

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
    QAction *about;
    QAction *aboutQt;
    QLabel *infoLabel;
    QPalette *palette;
    
    // Player options for dock widgets
    QAction *p1puckSelect;
    QAction *p2puckSelect;
   
 protected slots:
    void aboutCallback();     
    void aboutQtCallback();
    void getPlayer(Player&, int);

 signals:
   void setPlayer(Player, int);

 private:
    void createActions();
    void createMenus();
    void createDockWindows();

    bool ok;
};


#endif 

