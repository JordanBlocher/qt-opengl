#ifndef QGLAPP_H
#define QGLRAPP_H

#include <QMainWindow>

class QAction;
class QLabel;
class QMenu;
class QGLView;

class QGLApp : public QMainWindow
{
Q_OBJECT

 public:
    QGLApp(QWidget *parent = 0, QGLView *view = NULL);
    ~QGLApp();

    QGLView *glView;

 protected:
    virtual QMenu* createPopupMenu();
    
    QMenu *menu;
    QMenu *help;
    QAction *exit;
    QAction *about;
    QAction *aboutQt;
    QLabel *infoLabel;
   
 protected slots:
    void aboutCallback();     
    void aboutQtCallback();

 private:
    void createActions();
    void createMenus();

};


#endif 

