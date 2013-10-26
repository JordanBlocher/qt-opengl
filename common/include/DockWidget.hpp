#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QWidget>

class QCheckBox;
class QLabel;
class QErrorMessage;

class DockWidget : public QWidget
{
    Q_OBJECT

 public:
    DockWidget(QWidget *parent = 0);

 protected slots:
    void getPaddle(int);
    void player1();
    void player2();

 signals:
    void updatePaddle(const char*, int);

};

#endif
