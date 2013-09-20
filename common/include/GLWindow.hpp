#ifndef QGLAPP_H
#define QGLRAPP_H

#include <QMainWindow>

class QAction;
class QLabel;
class QMenu;
class GLViewport;
class GLOverlay;
class QResizeEvent;
class QColor;

class GLWindow : public QMainWindow
{
Q_OBJECT

 public:
    GLWindow(QWidget *parent = 0, GLViewport *view = NULL);
    ~GLWindow();

    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent*);

    GLViewport *glView;
    GLOverlay *overlay;

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

