#ifndef QGLAPP_H
#define QGLRAPP_H

#include <QMainWindow>

class QAction;
class QLabel;
class QMenu;
class GLViewport;
class OverlayWidget;
class QResizeEvent;
class QColor;

class MainWindow : public QMainWindow
{
Q_OBJECT

 public:
    MainWindow(QWidget *parent = 0, GLViewport *view = NULL);
    ~MainWindow();

    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent*);
    void mousePressEvent(QMouseEvent*);

    GLViewport *glView;
    OverlayWidget *overlay;

 protected:
    virtual QMenu* createPopupMenu();
    
    QMenu *menu;
    QMenu *help;
    QAction *exit;
    QAction *about;
    QAction *aboutQt;
    QLabel *infoLabel;
    QPalette *palette;
   
 protected slots:
    void aboutCallback();     
    void aboutQtCallback();

 private:
    void createActions();
    void createMenus();

};


#endif 

